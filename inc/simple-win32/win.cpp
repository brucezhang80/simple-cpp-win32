#include	<Windows.h>
#include	<tchar.h>

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
        GetClassName(hChild, buf, sizeof(buf) - 1);
        if(0 == _strcmpi(buf, sClass)) {
            return	hChild;
        }

        hChild	= GetWindow(hChild, GW_HWNDNEXT);
    } while(hChild != NULL);

    return	NULL;
}
