#include	<Windows.h>
#include	<tchar.h>

#include	<memory>

#include	"res.h"

//
//	判断是否64位系统
//
bool	Win_Is64bitsSystem() {
    typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);

    SYSTEM_INFO si;
    LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress( GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");;
    if (NULL != fnGetNativeSystemInfo) {
        fnGetNativeSystemInfo(&si);
    } else {
        GetSystemInfo(&si);
    }
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
            si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ) {
        return true;
    }

    return false;
}

//
//	根据类名获取子窗口，同 FindWindowEx
//
HWND	Win_FindChildByClass(HWND hWnd, const char* sClass) {
    HWND	hChild	= GetWindow(hWnd, GW_CHILD);
    if(NULL == hChild) {
        return	hChild;
    }

    do {
        char	buf[MAX_PATH]	= {};
        GetClassNameA(hChild, buf, sizeof(buf) - 1);
        if(0 == _strcmpi(buf, sClass)) {
            return	hChild;
        }

        hChild	= GetWindow(hChild, GW_HWNDNEXT);
    } while(hChild != NULL);

    return	NULL;
}

bool	Win_SetRgn(HWND hWnd, RGNDATA* data, size_t size) {
    XFORM xform;
    xform.eM11 = (FLOAT) 1.0;
    xform.eM22 = (FLOAT) 1.0;
    xform.eM12 = (FLOAT) 0.0;
    xform.eM21 = (FLOAT) 0.0;
    xform.eDx  = (FLOAT) 0.0;
    xform.eDy  = (FLOAT) 0.0;

    HRGN rgn = ExtCreateRegion(&xform, DWORD(size), (RGNDATA*)data);
    return (FALSE != SetWindowRgn(hWnd, rgn, FALSE));
}

bool	Win_SetRgnFromFile(HWND hWnd, const char* file) {
    size_t	size	= 0;
    if(!Res_LoadFile(file, NULL, size)) {
        return	true;
    }
    std::auto_ptr<char>	data(new char[size]);
    if(!Res_LoadFile(file, data.get(), size)) {
        return	true;
    }

    return	Win_SetRgn(hWnd, (RGNDATA*)data.get(), size);
}

bool	Win_SetRgnFromRes(HWND hWnd, HMODULE hModule, const char* res_name, const char*	res_type) {
    void*	data;
    size_t	size;

    if(!Res_LoadResource(hModule, res_name, res_type, data, size)) {
        return	false;
    }

    return	Win_SetRgn(hWnd, (RGNDATA*)data, size);
}
