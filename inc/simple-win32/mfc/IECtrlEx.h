#pragma once

#include	<string>
#include	<afxocc.h>
#include	<mshtmhst.h>
#include	<mshtmdid.h>
#include	"simple/delegate.h"
#include	"IECtrl.h"

class IECtrlEx : public IECtrl {
protected:
    DECLARE_DYNCREATE(IECtrlEx)

public:
    typedef		delegate<void (IECtrlEx*, LPDISPATCH, VARIANT*, VARIANT*, VARIANT*, VARIANT*, VARIANT*, BOOL*)>
    EVENT_BEFORE_NAVIGATE;
    typedef		delegate<void (IECtrlEx*, LPDISPATCH, VARIANT*)>		EVENT_NAVIGATE_COMPLETED;
    typedef		delegate<void (IECtrlEx*, LPDISPATCH, VARIANT*)>		EVENT_DOCUMENT_COMPLETED;

    typedef		delegate<void (IECtrlEx*, LPDISPATCH*, BOOL*, unsigned long, LPCTSTR, LPCTSTR)>
    EVENT_NEW_WINDOW_EX;
    typedef		delegate<void (IECtrlEx*, BOOL)>						EVENT_PRIVACY_IMPACTED_STATE_CHANGE;
    typedef		delegate<void (IECtrlEx*, LPDISPATCH, VARIANT*, VARIANT*)>EVENT_UPDATE_PAGE_STATUS;
    typedef		delegate<void (IECtrlEx*, LPDISPATCH)>					EVENT_PRINT_TEMPLATE_TEARDOWN;
    typedef		delegate<void (IECtrlEx*, LPDISPATCH)>					EVENT_PRINT_TEMPLATE_INSTANTIATION;
    typedef		delegate<void (IECtrlEx*, LPDISPATCH, VARIANT*, VARIANT*, VARIANT*, BOOL*)>
    EVENT_NAVIGATE_ERROR;
    typedef		delegate<void (IECtrlEx*, BOOL*)>						EVENT_FILE_DOWNLOAD;
    typedef		delegate<void (IECtrlEx*, long)>						EVENT_SET_SECURE_LOCK_ICON;
    typedef		delegate<void (IECtrlEx*, long*, long*)>				EVENT_CLIENT_TO_HOST_WINDOW;
    typedef		delegate<void (IECtrlEx*, BOOL, BOOL*)>					EVENT_WINDOW_CLOSING;
    typedef		delegate<void (IECtrlEx*, long)>						EVENT_WINDOW_SET_HEIGHT;
    typedef		delegate<void (IECtrlEx*, long)>						EVENT_WINDOW_SET_WIDTH;
    typedef		delegate<void (IECtrlEx*, long)>						EVENT_WINDOW_SET_TOP;
    typedef		delegate<void (IECtrlEx*, long)>						EVENT_WINDOW_SET_LEFT;
    typedef		delegate<void (IECtrlEx*, BOOL)>						EVENT_WINDOW_SET_RESIZABLE;
    typedef		delegate<void (IECtrlEx*, BOOL)>						EVENT_THEATER_MODE;
    typedef		delegate<void (IECtrlEx*, BOOL)>						EVENT_FULL_SCREEN;
    typedef		delegate<void (IECtrlEx*, BOOL)>						EVENT_STATUS_BAR;
    typedef		delegate<void (IECtrlEx*, BOOL)>						EVENT_MENU_BAR;
    typedef		delegate<void (IECtrlEx*, BOOL)>						EVENT_TOOL_BAR;
    typedef		delegate<void (IECtrlEx*, BOOL)>						EVENT_VISIBLE;
    typedef		delegate<void (IECtrlEx*)>								EVENT_QUIT;
    typedef		delegate<void (IECtrlEx*, LPDISPATCH*, BOOL*)>			EVENT_NEW_WINDOW;
    typedef		delegate<void (IECtrlEx*, LPCTSTR)>						EVENT_PROPERTY_CHANGE;
    typedef		delegate<void (IECtrlEx*, LPCTSTR)>						EVENT_TITLE_CHANGE;
    typedef		delegate<void (IECtrlEx*)>								EVENT_DOWNLOAD_COMPLETE;
    typedef		delegate<void (IECtrlEx*)>								EVENT_DOWNLOAD_BEGIN;
    typedef		delegate<void (IECtrlEx*, long, BOOL)>					EVENT_COMMAND_STATE_CHANGE;
    typedef		delegate<void (IECtrlEx*, long, long)>					EVENT_PROGRESS_CHANGE;
    typedef		delegate<void (IECtrlEx*, LPCTSTR)>						EVENT_STATUS_TEXT_CHANGE;

    EVENT_BEFORE_NAVIGATE					evtBeforeNavigate;
    EVENT_NAVIGATE_COMPLETED				evtNavigateCompleted;
    EVENT_DOCUMENT_COMPLETED				evtDocumentCompleted;

    EVENT_NEW_WINDOW_EX						evtNewWindowEx;
    EVENT_PRIVACY_IMPACTED_STATE_CHANGE		evtPrivacyImpactedStateChange;
    EVENT_UPDATE_PAGE_STATUS				evtUpdatePageStatus;
    EVENT_PRINT_TEMPLATE_TEARDOWN			evtPrintTemplateTeardown;
    EVENT_PRINT_TEMPLATE_INSTANTIATION		evtPrintTemplateInstantiation;
    EVENT_NAVIGATE_ERROR					evtNavigateError;
    EVENT_FILE_DOWNLOAD						evtFileDownload;
    EVENT_SET_SECURE_LOCK_ICON				evtSetSecureLockIcon;
    EVENT_CLIENT_TO_HOST_WINDOW				evtClientToHostWindow;
    EVENT_WINDOW_CLOSING					evtWindowClosing;
    EVENT_WINDOW_SET_HEIGHT					evtWindowSetHeight;
    EVENT_WINDOW_SET_WIDTH					evtWindowSetWidth;
    EVENT_WINDOW_SET_TOP					evtWindowSetTop;
    EVENT_WINDOW_SET_LEFT					evtWindowSetLeft;
    EVENT_WINDOW_SET_RESIZABLE				evtWindowSetResizable;
    EVENT_THEATER_MODE						evtTheaterMode;
    EVENT_FULL_SCREEN						evtFullScreen;
    EVENT_STATUS_BAR						evtStatusBar;
    EVENT_MENU_BAR							evtMenuBar;
    EVENT_TOOL_BAR							evtToolBar;
    EVENT_VISIBLE							evtVisible;
    EVENT_QUIT								evtQuit;
    EVENT_NEW_WINDOW						evtNewWindow;
    EVENT_PROPERTY_CHANGE					evtPropertyChange;
    EVENT_TITLE_CHANGE						evtTitleChange;
    EVENT_DOWNLOAD_COMPLETE					evtDownloadComplete;
    EVENT_DOWNLOAD_BEGIN					evtDownloadBegin;
    EVENT_COMMAND_STATE_CHANGE				evtCommandStateChange;
    EVENT_PROGRESS_CHANGE					evtProgressChange;
    EVENT_STATUS_TEXT_CHANGE				evtStatusTextChange;

private:
    void									Internal_OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
    void									Internal_OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* URL);
    void									Internal_OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

    void									Internal_OnNewWindow3(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl);
    void									Internal_OnPrivacyImpactedStateChange(BOOL bImpacted);
    void									Internal_OnUpdatePageStatus(LPDISPATCH pDisp, VARIANT* nPage, VARIANT* fDone);
    void									Internal_OnPrintTemplateTeardown(LPDISPATCH pDisp);
    void									Internal_OnPrintTemplateInstantiation(LPDISPATCH pDisp);
    void									Internal_OnNavigateError(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel);
    void									Internal_OnFileDownload(BOOL* Cancel);
    void									Internal_OnSetSecureLockIcon(long SecureLockIcon);
    void									Internal_OnClientToHostWindow(long* CX, long* CY);
    void									Internal_OnWindowClosing(BOOL IsChildWindow, BOOL* Cancel);
    void									Internal_OnWindowSetHeight(long Height);
    void									Internal_OnWindowSetWidth(long Width);
    void									Internal_OnWindowSetTop(long Top);
    void									Internal_OnWindowSetLeft(long Left);
    void									Internal_OnWindowSetResizable(BOOL Resizable);
    void									Internal_OnTheaterMode(BOOL TheaterMode);
    void									Internal_OnFullScreen(BOOL FullScreen);
    void									Internal_OnStatusBar(BOOL StatusBar);
    void									Internal_OnMenuBar(BOOL MenuBar);
    void									Internal_OnToolBar(BOOL ToolBar);
    void									Internal_OnVisible(BOOL Visible);
    void									Internal_OnQuit();
    void									Internal_OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
    void									Internal_OnPropertyChange(LPCTSTR szProperty);
    void									Internal_OnTitleChange(LPCTSTR Text);
    void									Internal_OnDownloadComplete();
    void									Internal_OnDownloadBegin();
    void									Internal_OnCommandStateChange(long Command, BOOL Enable);
    void									Internal_OnProgressChange(long Progress, long ProgressMax);
    void									Internal_OnStatusTextChange(LPCTSTR Text);

protected:
    DECLARE_EVENTSINK_MAP()
};

//
//	自定义控件Site，使用方式：
//
//	重载容器（对话框）的CreateControlSite方法，如下：
//
//		virtual BOOL CreateControlSite(COleControlContainer* pContainer, COleControlSite** ppSite, UINT /* nID */, REFCLSID /* clsid */)
//		{
//			if(ppSite == NULL)
//			{
//				ASSERT(FALSE);
//				return FALSE;
//			}
//
//			IECtrlEx_ControlSite_DocHostShowUI*	pSite	=	new IECtrlEx_ControlSite_DocHostShowUI(pContainer);
//			pSite->evtShowHelp		= bind(&WebBrowserContainerWnd::Internal_OnShowHelp, this);
//			pSite->evtShowMessage	= bind(&WebBrowserContainerWnd::Internal_OnShowMessage, this);
//			*ppSite = pSite;
//			return TRUE;
//		}
//
class IECtrlEx_ControlSite_DocHostShowUI : public COleControlSite, public IDocHostShowUI {
public:
    IECtrlEx_ControlSite_DocHostShowUI(COleControlContainer* pCtrlCont):	COleControlSite(pCtrlCont) {
    }

public:
    typedef		delegate<bool(HWND, LPOLESTR, LPOLESTR, DWORD, LPOLESTR, DWORD, LRESULT*)>	EVENT_SHOW_MESSAGE;
    typedef		delegate<bool(HWND, LPOLESTR, UINT, DWORD, POINT, IDispatch*)>				EVENT_SHOW_HELP;

    EVENT_SHOW_MESSAGE							evtShowMessage;
    EVENT_SHOW_HELP								evtShowHelp;

protected:
    // Implementation
    LPUNKNOWN GetInterfaceHook(const void *iid) {
        if (IsEqualIID((REFIID) (*(IID*)iid), IID_IDocHostShowUI)) {
            return (IDocHostShowUI *) this;
        }
        return NULL;
    }

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject) {
        return COleControlSite::ExternalQueryInterface(&riid, ppvObject);
    }

    STDMETHOD_(ULONG, AddRef)() {
        return ExternalAddRef();
    }

    STDMETHOD_(ULONG, Release)() {
        return ExternalRelease();
    }

protected:	// IDocHostShowUI Impls.
    virtual HRESULT STDMETHODCALLTYPE ShowMessage(
        /* [in] */ HWND hwnd,
        /* [in] */ LPOLESTR lpstrText,
        /* [in] */ LPOLESTR lpstrCaption,
        /* [in] */ DWORD dwType,
        /* [in] */ LPOLESTR lpstrHelpFile,
        /* [in] */ DWORD dwHelpContext,
        /* [out] */ LRESULT *plResult) {
        return	evtShowMessage && evtShowMessage(hwnd, lpstrText, lpstrCaption, dwType, lpstrHelpFile, dwHelpContext, plResult)
                ?	S_OK : S_FALSE;
    }

    virtual HRESULT STDMETHODCALLTYPE ShowHelp(
        /* [in] */ HWND hwnd,
        /* [in] */ LPOLESTR pszHelpFile,
        /* [in] */ UINT uCommand,
        /* [in] */ DWORD dwData,
        /* [in] */ POINT ptMouse,
        /* [out] */ IDispatch *pDispatchObjectHit) {
        return	evtShowHelp && evtShowHelp(hwnd, pszHelpFile, uCommand, dwData, ptMouse, pDispatchObjectHit)
                ?	S_OK : S_FALSE;
    }
};

//
//	WebBrowser容器，用于无法直接修改对话框的情形
//
class WebBrowserCtrlEx_ContainerWnd	: public CDialog {
public:
    void			Create(CWnd* pParentWnd, int nWidth, int nHeight, DWORD dwStyle, DWORD dwStyleEx = 0);

public:
    typedef			delegate<bool(DWORD&)>										EVENT_DOWNLOAD_CONTROL;
    typedef			delegate<bool(std::string&)>								EVENT_FETCH_USER_AGENT;

    EVENT_DOWNLOAD_CONTROL														evtDownloadControl;
    EVENT_FETCH_USER_AGENT														evtFetchUserAgent;

    typedef			IECtrlEx_ControlSite_DocHostShowUI::EVENT_SHOW_MESSAGE	EVENT_SHOW_MESSAGE;
    typedef			IECtrlEx_ControlSite_DocHostShowUI::EVENT_SHOW_HELP		EVENT_SHOW_HELP;

    EVENT_SHOW_MESSAGE															evtShowMessage;
    EVENT_SHOW_HELP																evtShowHelp;

protected:
    virtual			IECtrlEx_ControlSite_DocHostShowUI*	Internal_CreateControlSite(COleControlContainer* pContainer);

public:
    virtual BOOL	CreateControlSite(COleControlContainer* pContainer, COleControlSite** ppSite, UINT /* nID */, REFCLSID /* clsid */);
    virtual BOOL	OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);
    virtual BOOL	OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
    bool			Internal_OnShowMessage(
        /* [in] */ HWND hwnd,
        /* [in] */ LPOLESTR lpstrText,
        /* [in] */ LPOLESTR lpstrCaption,
        /* [in] */ DWORD dwType,
        /* [in] */ LPOLESTR lpstrHelpFile,
        /* [in] */ DWORD dwHelpContext,
        /* [out] */ LRESULT *plResult);

    bool			Internal_OnShowHelp(
        /* [in] */ HWND hwnd,
        /* [in] */ LPOLESTR pszHelpFile,
        /* [in] */ UINT uCommand,
        /* [in] */ DWORD dwData,
        /* [in] */ POINT ptMouse,
        /* [out] */ IDispatch *pDispatchObjectHit);
};
