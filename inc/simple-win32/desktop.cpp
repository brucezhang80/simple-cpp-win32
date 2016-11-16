#include	<Windows.h>
#include	<CommCtrl.h>

#include	"win.h"
#include	"desktop.h"

HWND	Desktop_FindSHELLDLLDefView(const char* sClass) {
    HWND	hView	= NULL;
    HWND	hWnd	= Win_FindChildByClass(GetDesktopWindow(), sClass);
    while(NULL != hWnd) {
        hView	= Win_FindChildByClass(hWnd, "SHELLDLL_DefView");
        if(NULL != hView) {
            break;
        }

        do {
            char	buf[MAX_PATH]	= {};
            hWnd	= GetWindow(hWnd, GW_HWNDNEXT);
            GetClassName(hWnd, buf, sizeof(buf) - 1);
            if(0 == _strcmpi(buf, "WorkerW")) {
                break;
            }
        } while(hWnd != NULL);
    }

    return	hView;
}

//
//	获取桌面SysListView窗口句柄
//
HWND	Desktop_GetSysListView() {
    HWND	xView	= Desktop_FindSHELLDLLDefView("WorkerW");
    if(NULL == xView) {
        xView	= Desktop_FindSHELLDLLDefView("Progman");
    }

    if(NULL != xView) {
        xView	=	Win_FindChildByClass(xView, "SysListView32");
    }

    return	xView;
}

//
//	获取桌面图标矩形
//
bool	Desktop_GetIconRect(const char* psCaption, RECT* pRect) {
    HANDLE	hProcess	= NULL;
    HWND	hView		= Desktop_GetSysListView();
    {
        DWORD PID;
        GetWindowThreadProcessId(hView, &PID);
        hProcess=OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, PID);
        if(NULL == hProcess) {
            return	false;
        }
    }

    LVITEM	xItem,	*pRemoteItem		= NULL;
    char	sText[512],	*pRemoteText	= NULL;

    pRemoteItem	= (LVITEM*)	VirtualAllocEx(hProcess, NULL, sizeof(xItem) + 1, MEM_COMMIT, PAGE_READWRITE);
    pRemoteText	= (char*)	VirtualAllocEx(hProcess, NULL, sizeof(sText) + 1, MEM_COMMIT, PAGE_READWRITE);

    bool	bFound	= false;
    int		nSum	= ListView_GetItemCount(hView);
    for(int i = 0; i < nSum; ++i) {
        memset(&xItem, 0, sizeof(xItem));
        xItem.mask			= LVIF_TEXT;
        xItem.iItem			= i;
        xItem.iSubItem		= 0;
        xItem.pszText		= pRemoteText;
        xItem.cchTextMax	= sizeof(sText);

        WriteProcessMemory(hProcess, pRemoteItem, &xItem, sizeof(xItem), NULL);
        ::SendMessage(hView, LVM_GETITEM, 0, (LPARAM)pRemoteItem);
        ReadProcessMemory(hProcess, pRemoteText, sText, sizeof(sText), NULL);
        if(0 == _strcmpi(sText, psCaption)) {
            if(NULL != pRect) {
                memset(pRect, 0, sizeof(RECT));
                pRect->left	= LVIR_SELECTBOUNDS;
                WriteProcessMemory(hProcess, pRemoteText, pRect, sizeof(RECT), NULL);
                ::SendMessage(hView, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)pRemoteText);
                ReadProcessMemory(hProcess, pRemoteText, pRect, sizeof(RECT), NULL);
            }
            bFound	= true;
            break;
        }
    }

    VirtualFreeEx(hProcess, pRemoteItem, 0, MEM_RELEASE);
    VirtualFreeEx(hProcess, pRemoteText, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return	bFound;
}

