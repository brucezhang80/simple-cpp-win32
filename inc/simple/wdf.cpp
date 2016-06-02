#include <assert.h>
#include <string.h>
#include <windows.h>
#include "wdf.h"

static unsigned long string2id(const char *str) {
    int i;
    unsigned int v;
    static unsigned m[70];
    strncpy((char *)m,str,256);
    for (i=0; i<256/4 && m[i]; i++) ;
    m[i++]=0x9BE74448,m[i++]=0x66F42C48;
    v=0xF4FA8928;

    __asm {
        mov esi,0x37A8470E		//;x0=0x37A8470E
        mov edi,0x7758B42B		//;y0=0x7758B42B
        xor ecx,ecx
        _loop:
        mov ebx,0x267B0B11		//;w=0x267B0B11
        rol v,1
        lea eax,m
        xor ebx,v

        mov eax,[eax+ecx*4]
        mov edx,ebx
        xor esi,eax
        xor edi,eax

        add edx,edi
        or edx,0x2040801		//;a=0x2040801
        and edx,0xBFEF7FDF		//;c=0xBFEF7FDF

        mov eax,esi
        mul edx
        adc eax,edx
        mov edx,ebx
        adc eax,0

        add edx,esi
        or edx,0x804021			//;b=0x804021
        and edx,0x7DFEFBFF		//;d=0x7DFEFBFF

        mov esi,eax
        mov eax,edi
        mul edx

        add edx,edx
        adc eax,edx
        jnc _skip
        add eax,2
        _skip:
        inc ecx;
        mov edi,eax
        cmp ecx,i
        jnz _loop
        xor esi,edi
        mov v,esi
    }

    return v;
}

static DWORD string_id(const char *filename) {
    char buffer[256];
    int i;
    for (i=0; filename[i]; i++) {
        if (filename[i]>='A' && filename[i]<='Z') buffer[i]=filename[i]+'a'-'A';
        else if (filename[i]=='/') buffer[i]='\\';
        else buffer[i]=filename[i];
    }
    buffer[i]=0;
    return string2id(buffer);
}

static DWORD real_name(const char *filename) {
    int i;
    for (i=0; filename[i]; i++) {
        if (filename[i]=='/') {
            return string_id(filename+i+1);
        }
    }
    return 0;
}

struct WDFDataFileIndex {
    DWORD uid;
    DWORD offset;
    DWORD size;
    DWORD space;
};

struct WDFDataFileHeader {
    DWORD id;
    int number;
    unsigned offset;
};

WDFDataFile::~WDFDataFile() {
    close();
}

void WDFDataFile::close() {
    if (m_hFile!=INVALID_HANDLE_VALUE) {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
    if (m_BlockIndex!=0) {
        free(m_BlockIndex);
        m_BlockIndex=0;
    }
}

WDFDataFileIndex* WDFDataFile::search_file(DWORD id) const {
    int begin,end,middle;
    begin=0,end=m_BlockNumber-1;
    while (begin<=end) {
        middle=(begin+end)/2;
        if (m_BlockIndex[middle].uid==id) return &m_BlockIndex[middle];
        else if (m_BlockIndex[middle].uid<id) begin=middle+1;
        else end=middle-1;
    }
    return 0;
}

WDFDataFileIndex* WDFDataFile::search_file(const char* filename) const {
    if(NULL == filename) {
        return	NULL;
    }

    return search_file(real_name(filename));
}

bool WDFDataFile::open(const char *filename) {
    HANDLE f;
    WDFDataFileHeader header;
    DWORD bytes;
    if (m_hFile!=INVALID_HANDLE_VALUE) {
        close();
    }
    f = CreateFile(filename, GENERIC_READ,FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (f==INVALID_HANDLE_VALUE)
        return false;
    if (ReadFile(f,&header,sizeof(header),&bytes,0)==0) {
        CloseHandle(f);
        return false;
    }
    if (header.id != 'WDFP') {
        CloseHandle(f);
        return false;
    }
    m_BlockIndex = (WDFDataFileIndex*)malloc(sizeof(WDFDataFileIndex)*header.number);
    SetFilePointer(f,header.offset,0,FILE_BEGIN);
    if (ReadFile(f,m_BlockIndex,sizeof(WDFDataFileIndex)*header.number,&bytes,0)==0) {
        CloseHandle(f);
        free(m_BlockIndex);
        return false;
    }
    m_BlockNumber=header.number;
    m_hFile=f;
    return true;
}

WDFFile::WDFFile() : m_pDataPtr(0), m_dwOffset(0), m_hFile(0), m_dwDataSize(0) {
    m_hFile=INVALID_HANDLE_VALUE;
}

bool WDFFile::load(WDFDataFile& datafile, const char *filename) {
    HANDLE f;
    DWORD bytes;
    f=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if (f==INVALID_HANDLE_VALUE) {
        if(!datafile.valid()) {
            return	false;
        }

        DWORD fid=real_name(filename);
        WDFDataFileIndex * pf;
        pf=datafile.search_file(fid);
        if (pf==0) {
            return false;
        }
        f=datafile.file_handle();
        m_dwOffset=pf->offset;
        m_dwDataSize=pf->size;
        SetFilePointer(f,m_dwOffset,0,FILE_BEGIN);
    } else {
        m_dwOffset=0;
        m_dwDataSize=GetFileSize(f,0);
    }

    if (m_pDataPtr) free(m_pDataPtr);
    m_pDataPtr=malloc(m_dwDataSize+1);

    if (ReadFile(f,m_pDataPtr,m_dwDataSize,&bytes,0)==0) {
        free(m_pDataPtr);
        m_pDataPtr=0;
        if (m_dwOffset==0) CloseHandle(f);
        return false;
    }
    ((char *)m_pDataPtr)[m_dwDataSize]=0;

    if (m_dwOffset==0) CloseHandle(f);
    return true;
}

WDFFile::~WDFFile() {
    close();
    if (m_pDataPtr)
        free(m_pDataPtr);
    m_pDataPtr=0;
}

bool WDFFile::open(WDFDataFile& datafile, const char *filename) {
    HANDLE f;
    f=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if (f==INVALID_HANDLE_VALUE) {
        if(!datafile.valid()) {
            return	false;
        }

        DWORD fid=real_name(filename);
        WDFDataFileIndex * pf;
        pf=datafile.search_file(fid);
        if (pf==0) {
            return false;
        }
        m_hFile=datafile.file_handle();
        m_dwOffset=pf->offset;
        m_dwDataSize=pf->size;
        SetFilePointer(m_hFile,m_dwOffset,0,FILE_BEGIN);
    } else {
        m_dwOffset=0;
        m_dwDataSize=GetFileSize(f,0);
        m_hFile=f;
    }
    return true;
}

void WDFFile::close() {
    if (m_dwOffset==0) {
        if (m_hFile!=INVALID_HANDLE_VALUE) {
            CloseHandle(m_hFile);
        }
    } else m_dwOffset=0;
    m_hFile=INVALID_HANDLE_VALUE;
}

bool WDFFile::read(void *buffer,int s,int offset,int *read) {
    assert(m_hFile!=INVALID_HANDLE_VALUE);
    s=min(s,(int)(m_dwDataSize-offset));
    //	assert((unsigned)(offset+s)<=m_dwDataSize);
    DWORD bytes;
    SetFilePointer(m_hFile,m_dwOffset+offset,0,FILE_BEGIN);
    if (s==0) {
        if (read) *read=0;
        return true;
    }
    if (ReadFile(m_hFile,buffer,s,&bytes,0)==0) {
        //		CloseHandle(m_hFile);
        //		m_hFile=INVALID_HANDLE_VALUE;
        return false;
    }
    if (read) *read=bytes;
    return true;
}

DWORD WDFFile::tell() const {
    DWORD r;
    assert(m_hFile!=INVALID_HANDLE_VALUE);
    r=SetFilePointer(m_hFile,0,0,FILE_CURRENT);
    return r-m_dwOffset;
}

void WDFFile::skip(int s) {
    assert(m_hFile!=INVALID_HANDLE_VALUE);
    SetFilePointer(m_hFile,s,0,FILE_CURRENT);
}

bool WDFFile::read(void* buffer, int s, int *read) {
    assert(m_hFile != INVALID_HANDLE_VALUE);
    DWORD bytes,r;
    //	SetFilePointer(m_hFile,m_dwOffset,0,FILE_BEGIN);
    if (read) {
        r=SetFilePointer(m_hFile,0,0,FILE_CURRENT);
        s=min(s,(int)(m_dwDataSize-(r-m_dwOffset)));
    }
    if (ReadFile(m_hFile,buffer,s,&bytes,0)==0) {
        //		CloseHandle(m_hFile);
        //		m_hFile=INVALID_HANDLE_VALUE;
        return false;
    }
    if (read) *read=bytes;
    return true;
}
