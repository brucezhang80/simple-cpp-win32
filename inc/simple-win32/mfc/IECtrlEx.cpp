#include "stdafx.h"
#include "IECtrlEx.h"
#include "DlgTemplate.h"

/////////////////////////////////////////////////////////////////////////////
// CExplorer1

IMPLEMENT_DYNCREATE(IECtrlEx, CWnd)

/////////////////////////////////////////////////////////////////////////////
// IECtrlEx ÊôÐÔ

/////////////////////////////////////////////////////////////////////////////
// IECtrlEx ²Ù×÷
BEGIN_EVENTSINK_MAP(IECtrlEx, IECtrl)
ON_EVENT_REFLECT(IECtrlEx, 102,	Internal_OnStatusTextChange, VTS_BSTR)
ON_EVENT_REFLECT(IECtrlEx, 104,	Internal_OnDownloadComplete, VTS_NONE)
ON_EVENT_REFLECT(IECtrlEx, 105,	Internal_OnCommandStateChange, VTS_I4 VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 106,	Internal_OnDownloadBegin, VTS_NONE)
ON_EVENT_REFLECT(IECtrlEx, 108,	Internal_OnProgressChange, VTS_I4 VTS_I4)
ON_EVENT_REFLECT(IECtrlEx, 112,	Internal_OnPropertyChange, VTS_BSTR)
ON_EVENT_REFLECT(IECtrlEx, 113,	Internal_OnTitleChange, VTS_BSTR)

ON_EVENT_REFLECT(IECtrlEx, 225,	Internal_OnPrintTemplateInstantiation, VTS_DISPATCH)
ON_EVENT_REFLECT(IECtrlEx, 226,	Internal_OnPrintTemplateTeardown, VTS_DISPATCH)
ON_EVENT_REFLECT(IECtrlEx, 227,	Internal_OnUpdatePageStatus, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT)

ON_EVENT_REFLECT(IECtrlEx, 273,	Internal_OnNewWindow3, VTS_PDISPATCH VTS_PBOOL VTS_UI4 VTS_BSTR VTS_BSTR)
ON_EVENT_REFLECT(IECtrlEx, 272,	Internal_OnPrivacyImpactedStateChange, VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 271,	Internal_OnNavigateError, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
ON_EVENT_REFLECT(IECtrlEx, 270,	Internal_OnFileDownload, VTS_PBOOL)
ON_EVENT_REFLECT(IECtrlEx, 269,	Internal_OnSetSecureLockIcon, VTS_I4)
ON_EVENT_REFLECT(IECtrlEx, 268,	Internal_OnClientToHostWindow, VTS_PI4 VTS_PI4)
ON_EVENT_REFLECT(IECtrlEx, 267,	Internal_OnWindowSetHeight, VTS_I4)
ON_EVENT_REFLECT(IECtrlEx, 266,	Internal_OnWindowSetWidth, VTS_I4)
ON_EVENT_REFLECT(IECtrlEx, 265,	Internal_OnWindowSetTop, VTS_I4)
ON_EVENT_REFLECT(IECtrlEx, 264,	Internal_OnWindowSetLeft, VTS_I4)
ON_EVENT_REFLECT(IECtrlEx, 263,	Internal_OnWindowClosing, VTS_BOOL VTS_PBOOL)
ON_EVENT_REFLECT(IECtrlEx, 262,	Internal_OnWindowSetResizable, VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 260,	Internal_OnTheaterMode, VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 259,	Internal_OnDocumentComplete, VTS_DISPATCH VTS_PVARIANT)
ON_EVENT_REFLECT(IECtrlEx, 258,	Internal_OnFullScreen, VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 257,	Internal_OnStatusBar, VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 256,	Internal_OnMenuBar, VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 255,	Internal_OnToolBar, VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 254,	Internal_OnVisible, VTS_BOOL)
ON_EVENT_REFLECT(IECtrlEx, 253,	Internal_OnQuit, VTS_NONE)
ON_EVENT_REFLECT(IECtrlEx, 252,	Internal_OnNavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
ON_EVENT_REFLECT(IECtrlEx, 251,	Internal_OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
ON_EVENT_REFLECT(IECtrlEx, 250,	Internal_OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

void IECtrlEx::Internal_OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) {
    if(evtBeforeNavigate) {
        evtBeforeNavigate(this, pDisp, URL, Flags, TargetFrameName, PostData, Headers, Cancel);
    }
}

void IECtrlEx::Internal_OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* URL) {
    if(evtNavigateCompleted) {
        evtNavigateCompleted(this, pDisp, URL);
    }
}

void IECtrlEx::Internal_OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL) {
    if(evtDocumentCompleted) {
        evtDocumentCompleted(this, pDisp, URL);
    }
}

void IECtrlEx::Internal_OnNewWindow3(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl) {
    if(evtNewWindowEx) {
        evtNewWindowEx(this, ppDisp, Cancel, dwFlags, bstrUrlContext, bstrUrl);
    }
}

void IECtrlEx::Internal_OnPrivacyImpactedStateChange(BOOL bImpacted) {
    if(evtPrivacyImpactedStateChange) {
        evtPrivacyImpactedStateChange(this, bImpacted);
    }
}

void IECtrlEx::Internal_OnUpdatePageStatus(LPDISPATCH pDisp, VARIANT* nPage, VARIANT* fDone) {
    if(evtUpdatePageStatus) {
        evtUpdatePageStatus(this, pDisp, nPage, fDone);
    }
}

void IECtrlEx::Internal_OnPrintTemplateTeardown(LPDISPATCH pDisp) {
    if(evtPrintTemplateTeardown) {
        evtPrintTemplateTeardown(this, pDisp);
    }
}

void IECtrlEx::Internal_OnPrintTemplateInstantiation(LPDISPATCH pDisp) {
    if(evtPrintTemplateInstantiation) {
        evtPrintTemplateInstantiation(this, pDisp);
    }
}

void IECtrlEx::Internal_OnNavigateError(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel) {
    if(evtNavigateError) {
        evtNavigateError(this, pDisp, URL, Frame, StatusCode, Cancel);
    }
}

void IECtrlEx::Internal_OnFileDownload(BOOL* Cancel) {
    if(evtFileDownload) {
        evtFileDownload(this, Cancel);
    }
}

void IECtrlEx::Internal_OnSetSecureLockIcon(long SecureLockIcon) {
    if(evtSetSecureLockIcon) {
        evtSetSecureLockIcon(this, SecureLockIcon);
    }
}

void IECtrlEx::Internal_OnClientToHostWindow(long* CX, long* CY) {
    if(evtClientToHostWindow) {
        evtClientToHostWindow(this, CX, CY);
    }
}

void IECtrlEx::Internal_OnWindowClosing(BOOL IsChildWindow, BOOL* Cancel) {
    if(evtWindowClosing) {
        evtWindowClosing(this, IsChildWindow, Cancel);
    }
}

void IECtrlEx::Internal_OnWindowSetHeight(long Height) {
    if(evtWindowSetHeight) {
        evtWindowSetHeight(this, Height);
    }
}

void IECtrlEx::Internal_OnWindowSetWidth(long Width) {
    if(evtWindowSetWidth) {
        evtWindowSetWidth(this, Width);
    }
}

void IECtrlEx::Internal_OnWindowSetTop(long Top) {
    if(evtWindowSetTop) {
        evtWindowSetTop(this, Top);
    }
}

void IECtrlEx::Internal_OnWindowSetLeft(long Left) {
    if(evtWindowSetLeft) {
        evtWindowSetLeft(this, Left);
    }
}

void IECtrlEx::Internal_OnWindowSetResizable(BOOL Resizable) {
    if(evtWindowSetResizable) {
        evtWindowSetResizable(this, Resizable);
    }
}

void IECtrlEx::Internal_OnTheaterMode(BOOL TheaterMode) {
    if(evtTheaterMode) {
        evtTheaterMode(this, TheaterMode);
    }
}

void IECtrlEx::Internal_OnFullScreen(BOOL FullScreen) {
    if(evtFullScreen) {
        evtFullScreen(this, FullScreen);
    }
}

void IECtrlEx::Internal_OnStatusBar(BOOL StatusBar) {
    if(evtStatusBar) {
        evtStatusBar(this, StatusBar);
    }
}

void IECtrlEx::Internal_OnMenuBar(BOOL MenuBar) {
    if(evtMenuBar) {
        evtMenuBar(this, MenuBar);
    }
}

void IECtrlEx::Internal_OnToolBar(BOOL ToolBar) {
    if(evtToolBar) {
        evtToolBar(this, ToolBar);
    }
}

void IECtrlEx::Internal_OnVisible(BOOL Visible) {
    if(evtVisible) {
        evtVisible(this, Visible);
    }
}

void IECtrlEx::Internal_OnQuit() {
    if(evtQuit) {
        evtQuit(this);
    }
}

void IECtrlEx::Internal_OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel) {
    if(evtNewWindow) {
        evtNewWindow(this, ppDisp, Cancel);
    }
}

void IECtrlEx::Internal_OnPropertyChange(LPCTSTR szProperty) {
    if(evtPropertyChange) {
        evtPropertyChange(this, szProperty);
    }
}

void IECtrlEx::Internal_OnTitleChange(LPCTSTR Text) {
    if(evtTitleChange) {
        evtTitleChange(this, Text);
    }
}

void IECtrlEx::Internal_OnDownloadComplete() {
    if(evtDownloadComplete) {
        evtDownloadComplete(this);
    }
}

void IECtrlEx::Internal_OnDownloadBegin() {
    if(evtDownloadBegin) {
        evtDownloadBegin(this);
    }
}

void IECtrlEx::Internal_OnCommandStateChange(long Command, BOOL Enable) {
    if(evtCommandStateChange) {
        evtCommandStateChange(this, Command, Enable);
    }
}

void IECtrlEx::Internal_OnProgressChange(long Progress, long ProgressMax) {
    if(evtProgressChange) {
        evtProgressChange(this, Progress, ProgressMax);
    }
}

void IECtrlEx::Internal_OnStatusTextChange(LPCTSTR Text) {
    if(evtStatusTextChange) {
        evtStatusTextChange(this, Text);
    }
}

void WebBrowserCtrlEx_ContainerWnd::Create(CWnd* pParentWnd, int nWidth, int nHeight, DWORD dwStyle, DWORD dwStyleEx) {
    RECT	rc	= {0, 0, nWidth, nHeight};

    DlgTemplate	m_DlgBuilder;
    m_DlgBuilder.Begin(dwStyle, rc, "", dwStyleEx);

    CreateIndirect(m_DlgBuilder.GetTemplate(), pParentWnd);
}

IECtrlEx_ControlSite_DocHostShowUI*	WebBrowserCtrlEx_ContainerWnd::Internal_CreateControlSite(COleControlContainer* pContainer) {
    return	new IECtrlEx_ControlSite_DocHostShowUI(pContainer);
}

BOOL WebBrowserCtrlEx_ContainerWnd::CreateControlSite(COleControlContainer* pContainer, COleControlSite** ppSite, UINT /* nID */, REFCLSID /* clsid */) {
    if(ppSite == NULL) {
        ASSERT(FALSE);
        return FALSE;
    }

    IECtrlEx_ControlSite_DocHostShowUI*	pSite	=	Internal_CreateControlSite(pContainer);
    pSite->evtShowHelp		= fastdelegate::bind(&WebBrowserCtrlEx_ContainerWnd::Internal_OnShowHelp, this);
    pSite->evtShowMessage	= fastdelegate::bind(&WebBrowserCtrlEx_ContainerWnd::Internal_OnShowMessage, this);
    *ppSite = pSite;
    return TRUE;
}

BOOL WebBrowserCtrlEx_ContainerWnd::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar) {
    DWORD	dwFlags	= 0;
    if(DISPID_AMBIENT_DLCONTROL == dispid && evtDownloadControl && evtDownloadControl(dwFlags)) {
        pvar->vt	= VT_I4;
        pvar->lVal	= dwFlags;
        return TRUE;
    }

    std::string	sAgent;
    if(DISPID_AMBIENT_USERAGENT == dispid && evtFetchUserAgent && evtFetchUserAgent(sAgent)) {
        CComBSTR strUserAgent(sAgent.c_str());

        pvar->vt = VT_BSTR;
        pvar->bstrVal = strUserAgent.Detach();
        return TRUE;
    }

    return __super::OnAmbientProperty(pSite, dispid, pvar);
}

BOOL WebBrowserCtrlEx_ContainerWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
    if(WM_ERASEBKGND == message) {
        *pResult	= TRUE;
        return	TRUE;
    }

    return	__super::OnWndMsg(message, wParam, lParam, pResult);
}

bool WebBrowserCtrlEx_ContainerWnd::Internal_OnShowMessage(
    /* [in] */ HWND hwnd,
    /* [in] */ LPOLESTR lpstrText,
    /* [in] */ LPOLESTR lpstrCaption,
    /* [in] */ DWORD dwType,
    /* [in] */ LPOLESTR lpstrHelpFile,
    /* [in] */ DWORD dwHelpContext,
    /* [out] */ LRESULT *plResult) {
    return	evtShowMessage && evtShowMessage(hwnd, lpstrText, lpstrCaption, dwType, lpstrHelpFile, dwHelpContext, plResult);
}

bool WebBrowserCtrlEx_ContainerWnd::Internal_OnShowHelp(
    /* [in] */ HWND hwnd,
    /* [in] */ LPOLESTR pszHelpFile,
    /* [in] */ UINT uCommand,
    /* [in] */ DWORD dwData,
    /* [in] */ POINT ptMouse,
    /* [out] */ IDispatch *pDispatchObjectHit) {
    return	evtShowHelp && evtShowHelp(hwnd, pszHelpFile, uCommand, dwData, ptMouse, pDispatchObjectHit);
}
