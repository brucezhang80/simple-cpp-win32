#pragma once

class ATL_NO_VTABLE IEHostWindow
    :	public	CAxHostWindow
    ,	public	IDocHostShowUI {
public:
    IEHostWindow(void);
    ~IEHostWindow(void);

public:
    typedef	HRESULT (WINAPI *FUNC_CreateInstance)(
        _In_opt_ void* pv,
        _In_ REFIID riid,
        _Deref_out_ LPVOID* ppv);

    static	void	patch_atl_creator_CAxHostWindow(FUNC_CreateInstance new_func = NULL, FUNC_CreateInstance* old_func = NULL);
    static	void	unpatch_atl_creator_CAxHostWindow();

public:
    DECLARE_POLY_AGGREGATABLE(IEHostWindow);

    BEGIN_COM_MAP(IEHostWindow)
    COM_INTERFACE_ENTRY2(IDispatch, IAxWinAmbientDispatchEx)
    COM_INTERFACE_ENTRY(IAxWinHostWindow)
    COM_INTERFACE_ENTRY(IAxWinHostWindowLic)
    COM_INTERFACE_ENTRY(IOleClientSite)
    COM_INTERFACE_ENTRY(IOleInPlaceSiteWindowless)
    COM_INTERFACE_ENTRY(IOleInPlaceSiteEx)
    COM_INTERFACE_ENTRY(IOleInPlaceSite)
    COM_INTERFACE_ENTRY(IOleWindow)
    COM_INTERFACE_ENTRY(IOleControlSite)
    COM_INTERFACE_ENTRY(IOleContainer)
    COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY(IServiceProvider)
    COM_INTERFACE_ENTRY(IAxWinAmbientDispatchEx)
    COM_INTERFACE_ENTRY(IAxWinAmbientDispatch)
#ifndef _ATL_NO_DOCHOSTUIHANDLER
    COM_INTERFACE_ENTRY(IDocHostUIHandler)
#endif
    COM_INTERFACE_ENTRY(IAdviseSink)
    COM_INTERFACE_ENTRY(IDocHostShowUI)
    END_COM_MAP()

protected:
    // IDocHostShowUI Impls.
    virtual HRESULT STDMETHODCALLTYPE ShowMessage(
        /* [in] */ HWND hwnd,
        /* [in] */ LPOLESTR lpstrText,
        /* [in] */ LPOLESTR lpstrCaption,
        /* [in] */ DWORD dwType,
        /* [in] */ LPOLESTR lpstrHelpFile,
        /* [in] */ DWORD dwHelpContext,
        /* [out] */ LRESULT *plResult);

    virtual HRESULT STDMETHODCALLTYPE ShowHelp(
        /* [in] */ HWND hwnd,
        /* [in] */ LPOLESTR pszHelpFile,
        /* [in] */ UINT uCommand,
        /* [in] */ DWORD dwData,
        /* [in] */ POINT ptMouse,
        /* [out] */ IDispatch *pDispatchObjectHit);
};

