#include <string>
#include <vector>

#include <windows.h>

#include "simple/string.h"

typedef struct tagHEADER {
    WORD idReserved;
    WORD idType;
    WORD idCount;
} HEADER, *LPHEADER;

typedef struct tagICONDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
} ICONDIRENTRY, *LPICONDIRENTRY;

#pragma pack( push )
#pragma pack( 2 )
typedef struct tagGRPICONDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    WORD nID;
} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;;

typedef struct tagGRPICONDIR {
    WORD idReserved;
    WORD idType;
    WORD idCount;
    GRPICONDIRENTRY idEntries[1];
} GRPICONDIR, *LPGRPICONDIR;
#pragma pack( pop )

bool Res_ReplaceICO(const char* lpExeName, const char* sResID, const char* lpIconFile) {
    LPICONDIRENTRY pIconDirEntry(NULL);
    LPGRPICONDIR pGrpIconDir(NULL);
    HEADER header;
    LPBYTE pIconBytes(NULL);
    HANDLE hIconFile(NULL);
    DWORD dwRet(0), nSize(0), nGSize(0), dwReserved(0);
    HANDLE hUpdate(NULL);
    BOOL ret(FALSE);
    WORD i(0);

    //打开图标文件
    hIconFile = CreateFile(lpIconFile, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hIconFile == INVALID_HANDLE_VALUE) {
        return	false;
    }
    //读取文件头部信息
    ret=ReadFile(hIconFile, &header, sizeof(HEADER), &dwReserved, NULL);
    if (!ret) {
        CloseHandle(hIconFile);
        return	false;
    }
    //建立每一个图标的目录信息存放区域
    pIconDirEntry = (LPICONDIRENTRY)new BYTE[header.idCount*sizeof(ICONDIRENTRY)];
    if (pIconDirEntry==NULL) {
        CloseHandle(hIconFile);
        return	false;
    }
    //从Icon文件中读取每一个图标的目录信息
    ret = ReadFile(hIconFile, pIconDirEntry, header.idCount*sizeof(ICONDIRENTRY), &dwReserved, NULL);
    if (!ret) {
        delete[] pIconDirEntry;
        CloseHandle(hIconFile);
        return	false;
    }
    //建立EXE文件中RT_GROUP_ICON所需的数据结构存放区域
    nGSize=sizeof(GRPICONDIR)+header.idCount*sizeof(ICONDIRENTRY);
    pGrpIconDir=(LPGRPICONDIR)new BYTE[nGSize];
    ZeroMemory(pGrpIconDir, nSize);
    //填充信息，这里相当于一个转换的过程
    pGrpIconDir->idReserved=header.idReserved;
    pGrpIconDir->idType=header.idType;
    pGrpIconDir->idCount=header.idCount;
    //复制信息并设置每一个图标对应的ID。ID为位置索引号
    for(i=0; i<header.idCount; i++) {
        pGrpIconDir->idEntries[i].bWidth=pIconDirEntry[i].bWidth;
        pGrpIconDir->idEntries[i].bHeight=pIconDirEntry[i].bHeight;
        pGrpIconDir->idEntries[i].bColorCount=pIconDirEntry[i].bColorCount;
        pGrpIconDir->idEntries[i].bReserved=pIconDirEntry[i].bReserved;
        pGrpIconDir->idEntries[i].wPlanes=pIconDirEntry[i].wPlanes;
        pGrpIconDir->idEntries[i].wBitCount=pIconDirEntry[i].wBitCount;
        pGrpIconDir->idEntries[i].dwBytesInRes=pIconDirEntry[i].dwBytesInRes;
        pGrpIconDir->idEntries[i].nID=i+1; //id == 0 是 RT_GROUP_ICON 的id，我这里替换的时候出现问题，所以就 + 1 了。
    }
    //开始更新EXE中的图标资源，ID定为最小０，如果原来存在０ID的图标信息则被替换为新的。
    hUpdate = BeginUpdateResource(lpExeName, false);
    if (hUpdate!=NULL) {
        //首先更新RT_GROUP_ICON信息
        ret = UpdateResource(hUpdate, RT_GROUP_ICON, sResID, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPVOID)pGrpIconDir, nGSize);
        if (!ret) {
            delete[] pIconDirEntry;
            delete[] pGrpIconDir;
            CloseHandle(hIconFile);
            return	false;
        }

        //接着的是每一个Icon的信息存放
        for(i=0; i<header.idCount; i++) {
            //Icon的字节数
            nSize = pIconDirEntry[i].dwBytesInRes;
            //偏移文件的指针到当前图标的开始处
            dwRet=SetFilePointer(hIconFile, pIconDirEntry[i].dwImageOffset, NULL, FILE_BEGIN);
            if (dwRet==INVALID_SET_FILE_POINTER) {
                break;
            }
            //准备pIconBytes来存放文件里的Byte信息用于更新到EXE中。

            delete[] pIconBytes;

            pIconBytes = new BYTE[nSize];
            ZeroMemory(pIconBytes, nSize);
            ret = ReadFile(hIconFile, (LPVOID)pIconBytes, nSize, &dwReserved, NULL);
            if(!ret) {
                break;
            }
            //更新每一个ID对应的RT_ICON信息
            ret = UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(pGrpIconDir->idEntries[i].nID), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPVOID)pIconBytes, nSize);
            if(!ret) {
                break;
            }
        }
        //结束EXE资源的更新操作
        if (pIconBytes!=NULL) {
            delete[] pIconBytes;
        }

        if(!EndUpdateResource(hUpdate, false)) {
            return	false;
        }
    }

    //清理资源并关闭Icon文件，到此更新操作结束！
    delete	[]pGrpIconDir;
    delete	[]pIconDirEntry;
    CloseHandle(hIconFile);

    return	true;
}

bool Res_ReplaceString(const char* pszApp, int nResID, const char* pszText) {
    std::wstring	wText;
    string_ansi_to_utf16(pszText,	wText);

    int SourceId	= nResID;
    HINSTANCE hInst = ::LoadLibrary(pszApp);
    if(hInst == NULL) return false;

    std::vector<WCHAR> vStrTable;  // 资源数据

    int idStr = (SourceId-1)/16 + 1;  // 字符串表ID
    HRSRC hSrc = ::FindResource(hInst, MAKEINTRESOURCE(idStr), RT_STRING);
    if(hSrc != NULL) {
        LPVOID lpData = ::LockResource( ::LoadResource(hInst,hSrc) );
        DWORD dwSize = ::SizeofResource(hInst,hSrc);
        vStrTable.resize((dwSize+1)/sizeof(WCHAR));
        ::CopyMemory(&vStrTable[0],lpData,dwSize); // 取得数据
    }
    ::FreeLibrary(hInst);

    HANDLE hUpdateRes=BeginUpdateResource(pszApp,false);
    if(hUpdateRes == NULL) return false;  // 不能更新,浪费表情

    int nIndex = (SourceId-1)%16; // 字符串表中的位置

    UINT nPos = 1;
    for (int i = 0; i < nIndex; i++) {       // 移到我们要修改的位置
        if(vStrTable.size() <= nPos) vStrTable.resize(nPos+1);
        nPos += vStrTable[nPos];
        nPos++;
    }

    if(vStrTable.size()<=nPos) vStrTable.resize(nPos+1);
    // 删除原先数据
    if(vStrTable[nPos]>0) {
        std::vector<WCHAR>::iterator itrStart=vStrTable.begin();
        std::advance(itrStart,nPos+1);
        std::vector<WCHAR>::iterator itrEnd = itrStart;
        std::advance(itrEnd,vStrTable[nPos]);
        vStrTable.erase(itrStart,itrEnd);
    }

    int nLen = ::lstrlenW(wText.c_str());
    vStrTable[nPos] = (WCHAR)nLen;

    // 插入现在的数据
    if(nLen>0) {
        std::vector<WCHAR>::iterator itrStart=vStrTable.begin();
        std::advance(itrStart,nPos+1);
        vStrTable.insert(itrStart,wText.c_str(),wText.c_str()+nLen);
    }

    // 好了,可以替换了

    UpdateResource(hUpdateRes,
                   RT_STRING,
                   MAKEINTRESOURCE(idStr),
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // 注意我这里用的语言不一样:)
                   &vStrTable[0],
                   vStrTable.size()*sizeof(WCHAR));

    return (FALSE != EndUpdateResource(hUpdateRes,FALSE));
}
