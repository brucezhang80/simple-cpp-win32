
#include	"ie.h"

//#include	<mshtmdid.h>
//#include	<afxocc.h>
//#include	<shlwapi.h>
#include	<wininet.h>


IE_DocHostUIHandler::IE_DocHostUIHandler(bool bAutoDelete, bool bZeroRefCount)
    : refcount_(1), external_(NULL), auto_delete_(bAutoDelete) {
    if (bZeroRefCount) {
        refcount_ = 0;
    }
}

IE_DocHostUIHandler::~IE_DocHostUIHandler() {

}

bool		IE_DocHostUIHandler::AttachTo(IDispatch* pDoc) {
    if( NULL == pDoc ) {
        return false;
    }

    IHTMLDocument2 *doc;
    pDoc->QueryInterface(IID_IHTMLDocument2, (void**)&doc);

    if( NULL != doc ) {
        bool ret = AttachTo(doc);

        doc->Release();
        return ret;
    }
    return false;
}
bool		IE_DocHostUIHandler::AttachTo(IHTMLDocument2* pDoc) {
    if( NULL == pDoc ) {
        return false;
    }

    ICustomDoc *custdoc;
    pDoc->QueryInterface(IID_ICustomDoc, (void**)&custdoc);

    if( NULL != custdoc ) {
        custdoc->SetUIHandler(this);

        custdoc->Release();
        return true;
    }
    return false;
}
bool		IE_DocHostUIHandler::DetachFrom(IDispatch* pDoc) {
    if( NULL == pDoc ) {
        return false;
    }

    IHTMLDocument2 *doc;
    pDoc->QueryInterface(IID_IHTMLDocument2, (void**)&doc);

    if( NULL != doc ) {
        bool ret = DetachFrom(doc);

        doc->Release();
        return ret;
    }
    return false;
}
bool		IE_DocHostUIHandler::DetachFrom(IHTMLDocument2* pDoc) {
    if( NULL == pDoc ) {
        return false;
    }

    ICustomDoc *custdoc;
    pDoc->QueryInterface(IID_ICustomDoc, (void**)&custdoc);

    if( NULL != custdoc ) {
        custdoc->SetUIHandler(NULL);

        custdoc->Release();
        return true;
    }
    return false;
}

IDispatch*	IE_DocHostUIHandler::GetExternalDispatch() {
    return external_;
}
void		IE_DocHostUIHandler::SetExternalDispatch(IDispatch* obj) {
    external_ = obj;
}

// IDocHostUIHandler Impls.
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::QueryInterface(REFIID classid, void** intf) {
    if (classid == IID_IUnknown) {
        *intf = (IUnknown*)this;
        AddRef();
    } else if (classid == IID_IDocHostUIHandler) {
        *intf = (IDocHostUIHandler*)this;
        AddRef();
    } else if (classid == IID_IDocHostUIHandler2) {
        *intf = (IDocHostUIHandler2*)this;
        AddRef();
    } else {
        return E_NOINTERFACE;
    }
    return S_OK;
}
ULONG STDMETHODCALLTYPE IE_DocHostUIHandler::AddRef() {
    InterlockedIncrement(&refcount_);
    return refcount_;
}
ULONG STDMETHODCALLTYPE IE_DocHostUIHandler::Release() {
    InterlockedDecrement(&refcount_);
    if (refcount_ == 0 && auto_delete_) {
        delete this;
    }
    return refcount_;
}
//返回S_OK，屏蔽掉右键菜单
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::ShowContextMenu(
    /* [in] */ DWORD dwID,
    /* [in] */ POINT __RPC_FAR *ppt,
    /* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
    /* [in] */ IDispatch __RPC_FAR *pdispReserved) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::GetHostInfo(
    /* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo) {
    pInfo->dwFlags	|= DOCHOSTUIFLAG_DIALOG;
    return S_OK;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::ShowUI(
    /* [in] */ DWORD dwID,
    /* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
    /* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
    /* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
    /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::HideUI( void) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::UpdateUI( void) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::EnableModeless(
    /* [in] */ BOOL fEnable) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::OnDocWindowActivate(
    /* [in] */ BOOL fActivate) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::OnFrameWindowActivate(
    /* [in] */ BOOL fActivate) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::ResizeBorder(
    /* [in] */ LPCRECT prcBorder,
    /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
    /* [in] */ BOOL fRameWindow) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::TranslateAccelerator(
    /* [in] */ LPMSG lpMsg,
    /* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
    /* [in] */ DWORD nCmdID) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::GetOptionKeyPath(
    /* [out] */ LPOLESTR __RPC_FAR *pchKey,
    /* [in] */ DWORD dw) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::GetDropTarget(
    /* [in] */ IDropTarget __RPC_FAR *pDropTarget,
    /* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::GetExternal(
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) {
    if(ppDispatch == NULL) {
        return E_POINTER;
    }

    if( NULL != external_ ) {
        external_->AddRef();
        *ppDispatch	= external_;

        return S_OK;
    }

    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::TranslateUrl(
    /* [in] */ DWORD dwTranslate,
    /* [in] */ OLECHAR __RPC_FAR *pchURLIn,
    /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::FilterDataObject(
    /* [in] */ IDataObject __RPC_FAR *pDO,
    /* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet) {
    return S_FALSE;
}

// IDocHostUIHandler2 Impls.
HRESULT STDMETHODCALLTYPE IE_DocHostUIHandler::GetOverrideKeyPath(
    /* [out] */ LPOLESTR *pchKey,
    /* [in] */ DWORD dw) {
    return	S_FALSE;
}

//
//	获取缺省的 alert 对话框标题
//
bool		IE_GetMessageBoxCaptioin(std::string& str) {
    char	buffer[MAX_PATH]	= {0};

    //载入Shdoclc.dll 和IE消息框标题字符串
    HINSTANCE hinstSHDOCLC = LoadLibrary(TEXT("SHDOCLC.DLL"));
    if (NULL == hinstSHDOCLC) {
        // 载入模块错误 -- 尽可能安全地失败
        return	false;
    }

    LoadStringA(hinstSHDOCLC, 2213, buffer, sizeof(buffer));	// 2213 为 窗口标题"Microsoft Internet Explorer"的资源标识

    str	= buffer;

    //卸载Shdoclc.dll并且返回
    FreeLibrary(hinstSHDOCLC);
    return	true;
}

//
//	从字符串加载网页内容
//
bool IE_LoadFromString(LPDISPATCH pDocument, const char* pszText) {
    if(NULL == pszText || NULL == pDocument) {
        return false;
    }

    size_t		nTextLen	= strlen(pszText);
    HGLOBAL		hGlobal		= GlobalAlloc(GPTR, nTextLen + 1);
    if(NULL != hGlobal ) {
        void*	pGlobalText = ::GlobalLock(hGlobal);
        if(NULL != pGlobalText) {
            ::memcpy(pGlobalText, pszText, nTextLen);
            ::GlobalUnlock(hGlobal);

            HRESULT		hr;
            IStream*	pStream		= NULL;
            hr = CreateStreamOnHGlobal(hGlobal, FALSE, &pStream);
            if(SUCCEEDED(hr)) {
                // Query for IPersistStreamInit.
                IPersistStreamInit*		pPersistStreamInit = NULL;
                hr = pDocument->QueryInterface(IID_IPersistStreamInit,  (void**)&pPersistStreamInit);
                if(SUCCEEDED(hr)) {
                    // Initialize the document.
                    hr = pPersistStreamInit->InitNew();
                    if ( SUCCEEDED(hr) ) {
                        // Load the contents of the stream.
                        hr = pPersistStreamInit->Load( pStream );
                    }
                    pPersistStreamInit->Release();
                }
                pStream->Release();
            }
        }
        GlobalFree(hGlobal);
    }

    return true;
}

//
//	获取脚本函数对象
//
DispatchPtr	IE_GetScriptFunctions(LPDISPATCH pDocument) {
    do {
        if(NULL == pDocument) {
            break;
        }

        DispatchPtr	pDoc	= pDocument;
        DispatchPtr	pScript	= pDoc.Get("script");
        if(NULL == pScript.GetInterfacePtr()) {
            break;
        }

        return	pScript;
    } while (false);

    return	NULL;
};

//
//	设置当前进程用到的代理服务器
//	http=http://xxxx.xxx.xxx:nnnn
//
bool	IE_SetProxy(const char* pszProxy, const char* pszAppName) {
    HINTERNET	hInternet		= InternetOpenA(pszAppName,
                                  INTERNET_OPEN_TYPE_DIRECT,
                                  NULL, NULL, 0
                                        );

    INTERNET_PER_CONN_OPTIONA	options[3];
    {
        options[0].dwOption			= INTERNET_PER_CONN_FLAGS;
        options[0].Value.dwValue	= (NULL == pszProxy || 0 == pszProxy[0]) ? 0 : PROXY_TYPE_PROXY;
        options[0].Value.dwValue	|=PROXY_TYPE_DIRECT;

        options[1].dwOption			= INTERNET_PER_CONN_PROXY_SERVER;
        options[1].Value.pszValue	= (char*)pszProxy;

        options[2].dwOption			= INTERNET_PER_CONN_PROXY_BYPASS;
        options[2].Value.pszValue	= "local";
    }

    INTERNET_PER_CONN_OPTION_LISTA	list;
    unsigned   long					nSize   =   sizeof(list);
    {
        list.dwSize					= sizeof(list);
        list.pszConnection			= NULL;
        list.dwOptionCount			= sizeof(options)/sizeof(options[2]);
        list.dwOptionError			= 0;
        list.pOptions				= options;
    }

    if(!InternetSetOption(hInternet,
                          INTERNET_OPTION_PER_CONNECTION_OPTION,
                          &list,   nSize
                         )) {
        return	false;
    }

    InternetCloseHandle(hInternet);
    return	true;
}
