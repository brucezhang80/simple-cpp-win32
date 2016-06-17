#ifndef IE_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define IE_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<string>

#include	<mshtml.h>
#include	<mshtmhst.h>

#include	"dispatch_ptr.h"

//
//	IDocHostUIHandler2实现类，使用时可在WebBrowser的OnDocumentCompleted事件里调用AttachTo方法
//
class IE_DocHostUIHandler : public IDocHostUIHandler2 {
public:
    IE_DocHostUIHandler(bool bAutoDelete = false, bool bZeroRefCount = false);
    virtual ~IE_DocHostUIHandler();

public:
    bool		AttachTo(IDispatch* pDoc);
    bool		AttachTo(IHTMLDocument2* pDoc);
    bool		DetachFrom(IDispatch* pDoc);
    bool		DetachFrom(IHTMLDocument2* pDoc);

    IDispatch*	GetExternalDispatch();
    void		SetExternalDispatch(IDispatch* obj);

public:
    // IDocHostUIHandler Impls.
    virtual HRESULT STDMETHODCALLTYPE		QueryInterface(REFIID classid, void** intf);
    virtual ULONG STDMETHODCALLTYPE			AddRef();
    virtual ULONG STDMETHODCALLTYPE			Release();

    //返回S_OK，屏蔽掉右键菜单
    virtual HRESULT STDMETHODCALLTYPE		ShowContextMenu(
        /* [in] */ DWORD dwID,
        /* [in] */ POINT __RPC_FAR *ppt,
        /* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
        /* [in] */ IDispatch __RPC_FAR *pdispReserved);
    virtual HRESULT STDMETHODCALLTYPE		GetHostInfo(
        /* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);
    virtual HRESULT STDMETHODCALLTYPE		ShowUI(
        /* [in] */ DWORD dwID,
        /* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
        /* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
        /* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
        /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc);
    virtual HRESULT STDMETHODCALLTYPE		HideUI( void);
    virtual HRESULT STDMETHODCALLTYPE		UpdateUI( void);
    virtual HRESULT STDMETHODCALLTYPE		EnableModeless(
        /* [in] */ BOOL fEnable);
    virtual HRESULT STDMETHODCALLTYPE		OnDocWindowActivate(
        /* [in] */ BOOL fActivate);
    virtual HRESULT STDMETHODCALLTYPE		OnFrameWindowActivate(
        /* [in] */ BOOL fActivate);
    virtual HRESULT STDMETHODCALLTYPE		ResizeBorder(
        /* [in] */ LPCRECT prcBorder,
        /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
        /* [in] */ BOOL fRameWindow);
    virtual HRESULT STDMETHODCALLTYPE		TranslateAccelerator(
        /* [in] */ LPMSG lpMsg,
        /* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
        /* [in] */ DWORD nCmdID);
    virtual HRESULT STDMETHODCALLTYPE		GetOptionKeyPath(
        /* [out] */ LPOLESTR __RPC_FAR *pchKey,
        /* [in] */ DWORD dw);
    virtual HRESULT STDMETHODCALLTYPE		GetDropTarget(
        /* [in] */ IDropTarget __RPC_FAR *pDropTarget,
        /* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget);
    virtual HRESULT STDMETHODCALLTYPE		GetExternal(
        /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
    virtual HRESULT STDMETHODCALLTYPE		TranslateUrl(
        /* [in] */ DWORD dwTranslate,
        /* [in] */ OLECHAR __RPC_FAR *pchURLIn,
        /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
    virtual HRESULT STDMETHODCALLTYPE		FilterDataObject(
        /* [in] */ IDataObject __RPC_FAR *pDO,
        /* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet);

public:
    // IDocHostUIHandler2 Impls.
    virtual HRESULT STDMETHODCALLTYPE		GetOverrideKeyPath(
        /* [out] */ LPOLESTR *pchKey,
        /* [in] */ DWORD dw);

private:
    long		refcount_;
    bool		auto_delete_;
    IDispatch*	external_;
};

//
//	等待页面载入成功
//
template<typename WebBrowser>
void	IE_WaitForLoaded(WebBrowser& browser) {
    while(4 != browser.get_ReadyState()) {
        MSG msg;
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            GetMessage(&msg, NULL, 0, 0);
            DispatchMessage(&msg);
        }
    }
}

//
//	载入页面
//
template<typename WebBrowser>
void		IE_LoadPage(WebBrowser& browser, const char* pszPage, bool bWait = false) {
    variant_t	navFlag(navNoHistory);

    browser.Stop();
    browser.Navigate(pszPage, &navFlag, NULL, NULL, NULL);

    if(bWait) {
        IE_WaitForLoaded(browser);
    }
}

//
//	载入空页面
//
template<typename WebBrowser>
void		IE_LoadBlankPage(WebBrowser& browser, bool bWait = false) {
    IE_LoadPage(browser, "about:blank", bWait);
}

//
//	获取缺省的 alert 对话框标题
//
bool		IE_GetMessageBoxCaptioin(std::string& str);

//
//	从字符串加载网页内容
//
bool		IE_LoadFromString(LPDISPATCH pDocument, const char* pszText);

//
//	获取脚本函数对象
//
DispatchPtr	IE_GetScriptFunctions(LPDISPATCH pDocument);

//
//	设置当前进程用到的代理服务器
//	http=http://xxxx.xxx.xxx:nnnn
//
bool		IE_SetProxy(const char* pszProxy, const char* pszAppName = "App");

#endif
