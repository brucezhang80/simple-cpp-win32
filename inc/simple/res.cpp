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

    //��ͼ���ļ�
    hIconFile = CreateFile(lpIconFile, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hIconFile == INVALID_HANDLE_VALUE) {
        return	false;
    }
    //��ȡ�ļ�ͷ����Ϣ
    ret=ReadFile(hIconFile, &header, sizeof(HEADER), &dwReserved, NULL);
    if (!ret) {
        CloseHandle(hIconFile);
        return	false;
    }
    //����ÿһ��ͼ���Ŀ¼��Ϣ�������
    pIconDirEntry = (LPICONDIRENTRY)new BYTE[header.idCount*sizeof(ICONDIRENTRY)];
    if (pIconDirEntry==NULL) {
        CloseHandle(hIconFile);
        return	false;
    }
    //��Icon�ļ��ж�ȡÿһ��ͼ���Ŀ¼��Ϣ
    ret = ReadFile(hIconFile, pIconDirEntry, header.idCount*sizeof(ICONDIRENTRY), &dwReserved, NULL);
    if (!ret) {
        delete[] pIconDirEntry;
        CloseHandle(hIconFile);
        return	false;
    }
    //����EXE�ļ���RT_GROUP_ICON��������ݽṹ�������
    nGSize=sizeof(GRPICONDIR)+header.idCount*sizeof(ICONDIRENTRY);
    pGrpIconDir=(LPGRPICONDIR)new BYTE[nGSize];
    ZeroMemory(pGrpIconDir, nSize);
    //�����Ϣ�������൱��һ��ת���Ĺ���
    pGrpIconDir->idReserved=header.idReserved;
    pGrpIconDir->idType=header.idType;
    pGrpIconDir->idCount=header.idCount;
    //������Ϣ������ÿһ��ͼ���Ӧ��ID��IDΪλ��������
    for(i=0; i<header.idCount; i++) {
        pGrpIconDir->idEntries[i].bWidth=pIconDirEntry[i].bWidth;
        pGrpIconDir->idEntries[i].bHeight=pIconDirEntry[i].bHeight;
        pGrpIconDir->idEntries[i].bColorCount=pIconDirEntry[i].bColorCount;
        pGrpIconDir->idEntries[i].bReserved=pIconDirEntry[i].bReserved;
        pGrpIconDir->idEntries[i].wPlanes=pIconDirEntry[i].wPlanes;
        pGrpIconDir->idEntries[i].wBitCount=pIconDirEntry[i].wBitCount;
        pGrpIconDir->idEntries[i].dwBytesInRes=pIconDirEntry[i].dwBytesInRes;
        pGrpIconDir->idEntries[i].nID=i+1; //id == 0 �� RT_GROUP_ICON ��id���������滻��ʱ��������⣬���Ծ� + 1 �ˡ�
    }
    //��ʼ����EXE�е�ͼ����Դ��ID��Ϊ��С�������ԭ�����ڣ�ID��ͼ����Ϣ���滻Ϊ�µġ�
    hUpdate = BeginUpdateResource(lpExeName, false);
    if (hUpdate!=NULL) {
        //���ȸ���RT_GROUP_ICON��Ϣ
        ret = UpdateResource(hUpdate, RT_GROUP_ICON, sResID, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPVOID)pGrpIconDir, nGSize);
        if (!ret) {
            delete[] pIconDirEntry;
            delete[] pGrpIconDir;
            CloseHandle(hIconFile);
            return	false;
        }

        //���ŵ���ÿһ��Icon����Ϣ���
        for(i=0; i<header.idCount; i++) {
            //Icon���ֽ���
            nSize = pIconDirEntry[i].dwBytesInRes;
            //ƫ���ļ���ָ�뵽��ǰͼ��Ŀ�ʼ��
            dwRet=SetFilePointer(hIconFile, pIconDirEntry[i].dwImageOffset, NULL, FILE_BEGIN);
            if (dwRet==INVALID_SET_FILE_POINTER) {
                break;
            }
            //׼��pIconBytes������ļ����Byte��Ϣ���ڸ��µ�EXE�С�

            delete[] pIconBytes;

            pIconBytes = new BYTE[nSize];
            ZeroMemory(pIconBytes, nSize);
            ret = ReadFile(hIconFile, (LPVOID)pIconBytes, nSize, &dwReserved, NULL);
            if(!ret) {
                break;
            }
            //����ÿһ��ID��Ӧ��RT_ICON��Ϣ
            ret = UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(pGrpIconDir->idEntries[i].nID), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPVOID)pIconBytes, nSize);
            if(!ret) {
                break;
            }
        }
        //����EXE��Դ�ĸ��²���
        if (pIconBytes!=NULL) {
            delete[] pIconBytes;
        }

        if(!EndUpdateResource(hUpdate, false)) {
            return	false;
        }
    }

    //������Դ���ر�Icon�ļ������˸��²���������
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

    std::vector<WCHAR> vStrTable;  // ��Դ����

    int idStr = (SourceId-1)/16 + 1;  // �ַ�����ID
    HRSRC hSrc = ::FindResource(hInst, MAKEINTRESOURCE(idStr), RT_STRING);
    if(hSrc != NULL) {
        LPVOID lpData = ::LockResource( ::LoadResource(hInst,hSrc) );
        DWORD dwSize = ::SizeofResource(hInst,hSrc);
        vStrTable.resize((dwSize+1)/sizeof(WCHAR));
        ::CopyMemory(&vStrTable[0],lpData,dwSize); // ȡ������
    }
    ::FreeLibrary(hInst);

    HANDLE hUpdateRes=BeginUpdateResource(pszApp,false);
    if(hUpdateRes == NULL) return false;  // ���ܸ���,�˷ѱ���

    int nIndex = (SourceId-1)%16; // �ַ������е�λ��

    UINT nPos = 1;
    for (int i = 0; i < nIndex; i++) {       // �Ƶ�����Ҫ�޸ĵ�λ��
        if(vStrTable.size() <= nPos) vStrTable.resize(nPos+1);
        nPos += vStrTable[nPos];
        nPos++;
    }

    if(vStrTable.size()<=nPos) vStrTable.resize(nPos+1);
    // ɾ��ԭ������
    if(vStrTable[nPos]>0) {
        std::vector<WCHAR>::iterator itrStart=vStrTable.begin();
        std::advance(itrStart,nPos+1);
        std::vector<WCHAR>::iterator itrEnd = itrStart;
        std::advance(itrEnd,vStrTable[nPos]);
        vStrTable.erase(itrStart,itrEnd);
    }

    int nLen = ::lstrlenW(wText.c_str());
    vStrTable[nPos] = (WCHAR)nLen;

    // �������ڵ�����
    if(nLen>0) {
        std::vector<WCHAR>::iterator itrStart=vStrTable.begin();
        std::advance(itrStart,nPos+1);
        vStrTable.insert(itrStart,wText.c_str(),wText.c_str()+nLen);
    }

    // ����,�����滻��

    UpdateResource(hUpdateRes,
                   RT_STRING,
                   MAKEINTRESOURCE(idStr),
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // ע���������õ����Բ�һ��:)
                   &vStrTable[0],
                   vStrTable.size()*sizeof(WCHAR));

    return (FALSE != EndUpdateResource(hUpdateRes,FALSE));
}
