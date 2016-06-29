#include	<atlbase.h>
#include	<atlhost.h>

#include	"IEHostWindow.h"

#include	"simple-win32/patcher.h"

//////////////////////////////////////////////////////////
static	Patcher*	gs_pacher_CreateInstance	= NULL;
//////////////////////////////////////////////////////////

IEHostWindow::IEHostWindow(void) {
}


IEHostWindow::~IEHostWindow(void) {
}

void IEHostWindow::patch_atl_creator_CAxHostWindow(FUNC_CreateInstance new_func, FUNC_CreateInstance* old_func) {
    unpatch_atl_creator_CAxHostWindow();

    FUNC_CreateInstance	xold;
    gs_pacher_CreateInstance	= new Patcher(
        &CAxHostWindow::_CreatorClass::CreateInstance,
        (NULL != new_func ? new_func : &IEHostWindow::_CreatorClass::CreateInstance),
        &xold,
        true,
        false
    );
    if(NULL != old_func) {
        *old_func	= xold;
    }
}

void IEHostWindow::unpatch_atl_creator_CAxHostWindow() {
    if(NULL != gs_pacher_CreateInstance) {
        if(gs_pacher_CreateInstance->patched()) {
            gs_pacher_CreateInstance->remove_patch();
        }
        delete	gs_pacher_CreateInstance;
        gs_pacher_CreateInstance	= NULL;
    }
}

HRESULT STDMETHODCALLTYPE	IEHostWindow::ShowMessage(
    /* [in] */ HWND hwnd,
    /* [in] */ LPOLESTR lpstrText,
    /* [in] */ LPOLESTR lpstrCaption,
    /* [in] */ DWORD dwType,
    /* [in] */ LPOLESTR lpstrHelpFile,
    /* [in] */ DWORD dwHelpContext,
    /* [out] */ LRESULT *plResult) {
    return	(evtShowMessage && evtShowMessage(hwnd, lpstrText, lpstrCaption, dwType, lpstrHelpFile, dwHelpContext, plResult))
            ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE	IEHostWindow::ShowHelp(
    /* [in] */ HWND hwnd,
    /* [in] */ LPOLESTR pszHelpFile,
    /* [in] */ UINT uCommand,
    /* [in] */ DWORD dwData,
    /* [in] */ POINT ptMouse,
    /* [out] */ IDispatch *pDispatchObjectHit) {
    return	(evtShowHelp && evtShowHelp(hwnd, pszHelpFile, uCommand, dwData, ptMouse, pDispatchObjectHit))
            ? S_OK : S_FALSE;
}
