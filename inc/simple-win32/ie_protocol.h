#ifndef IE_PROTOCOL_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define IE_PROTOCOL_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<string>

#include	<mshtml.h>

#include	"dispatch_impl.h"

//
//IInternetSession*	m_pInternetSession;
//
//
//// 注册协议
//if(FAILED(CoInternetGetSession(0, &m_pInternetSession, 0)))
//{
//	return	-1;
//}
//
//if(FAILED(m_pInternetSession->RegisterNameSpace(
//	m_pAppProtocol, CLSID_NULL, gs_sProtocolName, 0, NULL, 0))
//	)
//{
//	// 协议注册失败的处理;
//	return	-1;
//}
//
//
//// 注销协议
//if(m_pInternetSession!=NULL)
//{
//	m_pInternetSession->UnregisterNameSpace(m_pAppProtocol, gs_sProtocolName);
//	m_pInternetSession->Release();
//	m_pInternetSession	= NULL;
//}
//

//
//	IE WebBrowser自定义协议支持
//
class	IE_CustomProtocol : public DispatchImpl
    ,	public IClassFactory
    ,	public IInternetProtocol
    ,	public IInternetProtocolInfo {
public:
    IE_CustomProtocol(bool bAutoDelete = false, bool bZeroRefCount = false);

protected:
    virtual	IE_CustomProtocol*			do_create_protocol();
    virtual	void						do_destroy_protocol(IE_CustomProtocol*);
    virtual	bool						do_object_load(const char* szUrl);
    virtual	std::wstring				do_object_mime_type(const char* szUrl);
    virtual	size_t						do_object_data_size(const char* szUrl);
    virtual	size_t						do_object_data_read(void *pv, size_t size, size_t offset);

public:
    // IUnknown 实现
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void ** ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef( void);
    virtual ULONG STDMETHODCALLTYPE Release( void);

public:
    //  IClassFactory 实现
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE CreateInstance(
        /* [unique][in] */ IUnknown *pUnkOuter,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void **ppvObject);
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE LockServer(
        /* [in] */ BOOL fLock);

public:
    //  IInternetProtocolRoot 实现
    virtual HRESULT STDMETHODCALLTYPE Start(
        /* [in] */ LPCWSTR szUrl,
        /* [in] */ IInternetProtocolSink *pOIProtSink,
        /* [in] */ IInternetBindInfo *pOIBindInfo,
        /* [in] */ DWORD grfPI,
        /* [in] */ HANDLE_PTR dwReserved);
    virtual HRESULT STDMETHODCALLTYPE Continue(
        /* [in] */ PROTOCOLDATA *pProtocolData);
    virtual HRESULT STDMETHODCALLTYPE Abort(
        /* [in] */ HRESULT hrReason,
        /* [in] */ DWORD dwOptions);
    virtual HRESULT STDMETHODCALLTYPE Terminate(
        /* [in] */ DWORD dwOptions);
    virtual HRESULT STDMETHODCALLTYPE Suspend( void);
    virtual HRESULT STDMETHODCALLTYPE Resume( void);

public:
    //  IInternetProtocol 实现
    virtual HRESULT STDMETHODCALLTYPE Read(
        /* [length_is][size_is][out][in] */ void *pv,
        /* [in] */ ULONG cb,
        /* [out] */ ULONG *pcbRead);
    virtual HRESULT STDMETHODCALLTYPE Seek(
        /* [in] */ LARGE_INTEGER dlibMove,
        /* [in] */ DWORD dwOrigin,
        /* [out] */ ULARGE_INTEGER *plibNewPosition);
    virtual HRESULT STDMETHODCALLTYPE LockRequest(
        /* [in] */ DWORD dwOptions);
    virtual HRESULT STDMETHODCALLTYPE UnlockRequest( void);

public:
    //  IInternetProtocolInfo 实现
    virtual HRESULT STDMETHODCALLTYPE ParseUrl(
        /* [in] */ LPCWSTR pwzUrl,
        /* [in] */ PARSEACTION ParseAction,
        /* [in] */ DWORD dwParseFlags,
        /* [out] */ LPWSTR pwzResult,
        /* [in] */ DWORD cchResult,
        /* [out] */ DWORD *pcchResult,
        /* [in] */ DWORD dwReserved);
    virtual HRESULT STDMETHODCALLTYPE CombineUrl(
        /* [in] */ LPCWSTR pwzBaseUrl,
        /* [in] */ LPCWSTR pwzRelativeUrl,
        /* [in] */ DWORD dwCombineFlags,
        /* [out] */ LPWSTR pwzResult,
        /* [in] */ DWORD cchResult,
        /* [out] */ DWORD *pcchResult,
        /* [in] */ DWORD dwReserved);
    virtual HRESULT STDMETHODCALLTYPE CompareUrl(
        /* [in] */ LPCWSTR pwzUrl1,
        /* [in] */ LPCWSTR pwzUrl2,
        /* [in] */ DWORD dwCompareFlags);
    virtual HRESULT STDMETHODCALLTYPE QueryInfo(
        /* [in] */ LPCWSTR pwzUrl,
        /* [in] */ QUERYOPTION OueryOption,
        /* [in] */ DWORD dwQueryFlags,
        /* [size_is][out][in] */ LPVOID pBuffer,
        /* [in] */ DWORD cbBuffer,
        /* [out][in] */ DWORD *pcbBuf,
        /* [in] */ DWORD dwReserved);

protected:
    size_t						m_nSize;
    size_t						m_nPos;
    IInternetProtocolSink*		m_pOIProtSink;

private:
    bool	m_bAutoDelete;

public:
    DISPATCH_ITEMS_BEGIN(IE_CustomProtocol)
    DISPATCH_ITEMS_END();
};

#endif
