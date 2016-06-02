
#include	<cassert>

#include	"ie_protocol.h"
#include	<shlwapi.h>


IE_CustomProtocol::IE_CustomProtocol(bool bAutoDelete, bool bZeroRefCount)
    :	DispatchImpl(bAutoDelete, bZeroRefCount)
    ,	m_bAutoDelete(bAutoDelete)
    ,	m_pOIProtSink(NULL)
    ,	m_nPos(0)
    ,	m_nSize(0) {
}

IE_CustomProtocol*	IE_CustomProtocol::do_create_protocol() {
    return	this;
}

void						IE_CustomProtocol::do_destroy_protocol(IE_CustomProtocol*) {
    // Nothing.
}

bool						IE_CustomProtocol::do_object_load(const char* szUrl) {
    return	false;
}

std::wstring				IE_CustomProtocol::do_object_mime_type(const char* szUrl) {
    static	const char*		s_mime_ext[]	=	{
        ".htm",
        ".html",
        ".dtd",
        ".xml",
        ".xsl",
        ".txt",
        ".js",
        ".css",
        ".bmp",
        ".gif",
        ".png",
        ".jpeg",
        ".jpg",
        ".jpe",
        ".svg",
        ".svgz",
        ".wav",
        ".mid",
        ".midi",
        ".mp3",
        ".mp2",
        ".mpeg",
        ".mpg",
        ".mpe",
        ".avi",
        ".swf",
    };
    static	const wchar_t*	s_mime_type[]	=	{
        L"text/html",
        L"text/html",
        L"text/plain",
        L"text/xml",
        L"text/xml",
        L"text/plain",
        L"text/javascript",
        L"text/css",
        L"image/bmp",
        L"image/gif",
        L"image/png",
        L"image/jpeg",
        L"image/jpeg",
        L"image/jpeg",
        L"image/svg+xml",
        L"image/svg+xml",
        L"audio/x-wav",
        L"audio/x-midi",
        L"audio/x-midi",
        L"video/mpeg",
        L"audio/x-mpeg",
        L"video/mpeg",
        L"video/mpeg",
        L"video/mpeg",
        L"video/x-msvideo",
        L"application/x-shockwave-flash",
    };

    const char*		pszExt	=	PathFindExtensionA(szUrl);
    for(int i = 0; i < sizeof(s_mime_ext)/sizeof(s_mime_ext[0]); ++i) {
        if(0 == _strcmpi(pszExt, s_mime_ext[i])) {
            return	s_mime_type[i];
        }
    }

    return	L"";
}

size_t						IE_CustomProtocol::do_object_data_size(const char* szUrl) {
    return	0;
}

size_t						IE_CustomProtocol::do_object_data_read(void *pv, size_t size, size_t offset) {
    return	0;
}

// IUnknown 实现
HRESULT STDMETHODCALLTYPE IE_CustomProtocol::QueryInterface(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void ** ppvObject) {
    if(riid == IID_IInternetProtocol) {
        DispatchImpl::AddRef();
        *ppvObject = (IInternetProtocol*)this;
        return S_OK;
    }

    if(riid == IID_IInternetProtocolRoot) {
        DispatchImpl::AddRef();
        *ppvObject = (IInternetProtocolRoot*)this;
        return S_OK;
    }

    if(riid == IID_IInternetProtocolInfo) {
        DispatchImpl::AddRef();
        *ppvObject = (IInternetProtocolInfo*)this;
        return S_OK;
    }

    return	DispatchImpl::QueryInterface(riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IE_CustomProtocol::AddRef( void) {
    return	DispatchImpl::AddRef();
}

ULONG STDMETHODCALLTYPE IE_CustomProtocol::Release( void) {
    return	DispatchImpl::Release();
}


//  IClassFactory 实现
/* [local] */ HRESULT STDMETHODCALLTYPE IE_CustomProtocol::CreateInstance(
    /* [unique][in] */ IUnknown *pUnkOuter,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppvObject) {
    // 通过清除调用者的句柄呈现一个错误
    *ppvObject = 0;

    // 不支持聚合
    if (NULL != pUnkOuter) {
        return	CLASS_E_NOAGGREGATION;
    }

    IE_CustomProtocol*	pObj	= do_create_protocol();
    HRESULT	hr	= pObj->QueryInterface(riid, ppvObject);
    if(FAILED(hr)) {
        do_destroy_protocol(pObj);
    }
    return	hr;
}

/* [local] */ HRESULT STDMETHODCALLTYPE IE_CustomProtocol::LockServer(
    /* [in] */ BOOL fLock) {
    return	S_OK;
}

//  IInternetProtocolRoot 实现
HRESULT STDMETHODCALLTYPE IE_CustomProtocol::Start(
    /* [in] */ LPCWSTR szUrl,
    /* [in] */ IInternetProtocolSink *pOIProtSink,
    /* [in] */ IInternetBindInfo *pOIBindInfo,
    /* [in] */ DWORD grfPI,
    /* [in] */ HANDLE_PTR dwReserved) {
    const char*	sURL	= NULL;
    {
        char	buf[1024*4]	= {0};
        size_t	size_c	= WideCharToMultiByte(CP_ACP, 0,
                                              szUrl,	-1,
                                              buf,	int(sizeof(buf)/sizeof(buf[0]) - 1),
                                              NULL, NULL
                                           );
        buf[size_c]= 0;
        sURL	= buf;
    }

    if(!do_object_load(sURL)) {
        return INET_E_DATA_NOT_AVAILABLE ;
    }

    m_nPos			= 0;
    m_pOIProtSink	= pOIProtSink;
    m_nSize			= do_object_data_size(sURL);

    m_pOIProtSink->ReportProgress(BINDSTATUS_FINDINGRESOURCE,				szUrl);
    m_pOIProtSink->ReportProgress(BINDSTATUS_CONNECTING,					szUrl);
    m_pOIProtSink->ReportProgress(BINDSTATUS_SENDINGREQUEST,				szUrl);
    m_pOIProtSink->ReportProgress(BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE,		do_object_mime_type(sURL).c_str());

    m_pOIProtSink->ReportData(BSCF_FIRSTDATANOTIFICATION, 0, m_nSize);
    m_pOIProtSink->ReportData(BSCF_LASTDATANOTIFICATION | BSCF_DATAFULLYAVAILABLE,
                              m_nSize,
                              m_nSize
                             );

    return	S_OK;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::Continue(
    /* [in] */ PROTOCOLDATA *pProtocolData) {
    return	S_OK;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::Abort(
    /* [in] */ HRESULT hrReason,
    /* [in] */ DWORD dwOptions) {
    return	S_OK;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::Terminate(
    /* [in] */ DWORD dwOptions) {
    return	S_OK;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::Suspend( void) {
    return	E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::Resume( void) {
    return	E_NOTIMPL;
}

//  IInternetProtocol 实现
HRESULT STDMETHODCALLTYPE IE_CustomProtocol::Read(
    /* [length_is][size_is][out][in] */ void *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbRead) {
    if(m_nPos >= m_nSize) {
        m_pOIProtSink->ReportResult(S_OK, 0, NULL);
        return	S_FALSE;
    }

    size_t	nSize	= m_nSize - m_nPos;
    if(nSize > cb) {
        nSize	= cb;
    }
    nSize	= do_object_data_read(pv, nSize, m_nPos);
    assert(cb >= nSize);

    *pcbRead	= nSize;
    m_nPos		+= nSize;

    return	m_nPos >= m_nSize ? S_FALSE : S_OK;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::Seek(
    /* [in] */ LARGE_INTEGER dlibMove,
    /* [in] */ DWORD dwOrigin,
    /* [out] */ ULARGE_INTEGER *plibNewPosition) {
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::LockRequest(
    /* [in] */ DWORD dwOptions) {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::UnlockRequest( void) {
    return S_OK;
}

//  IInternetProtocolInfo 实现
HRESULT STDMETHODCALLTYPE IE_CustomProtocol::ParseUrl(
    /* [in] */ LPCWSTR pwzUrl,
    /* [in] */ PARSEACTION ParseAction,
    /* [in] */ DWORD dwParseFlags,
    /* [out] */ LPWSTR pwzResult,
    /* [in] */ DWORD cchResult,
    /* [out] */ DWORD *pcchResult,
    /* [in] */ DWORD dwReserved) {
    return INET_E_DEFAULT_ACTION;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::CombineUrl(
    /* [in] */ LPCWSTR pwzBaseUrl,
    /* [in] */ LPCWSTR pwzRelativeUrl,
    /* [in] */ DWORD dwCombineFlags,
    /* [out] */ LPWSTR pwzResult,
    /* [in] */ DWORD cchResult,
    /* [out] */ DWORD *pcchResult,
    /* [in] */ DWORD dwReserved) {
    return INET_E_DEFAULT_ACTION;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::CompareUrl(
    /* [in] */ LPCWSTR pwzUrl1,
    /* [in] */ LPCWSTR pwzUrl2,
    /* [in] */ DWORD dwCompareFlags) {
    if (pwzUrl1 == NULL || pwzUrl2 == NULL) {
        return E_POINTER;
    }

    HRESULT hr = S_FALSE;
    if (0 == _wcsicmp(pwzUrl1, pwzUrl2)) {
        hr = S_OK;
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE IE_CustomProtocol::QueryInfo(
    /* [in] */ LPCWSTR pwzUrl,
    /* [in] */ QUERYOPTION OueryOption,
    /* [in] */ DWORD dwQueryFlags,
    /* [size_is][out][in] */ LPVOID pBuffer,
    /* [in] */ DWORD cbBuffer,
    /* [out][in] */ DWORD *pcbBuf,
    /* [in] */ DWORD dwReserved) {
    return INET_E_DEFAULT_ACTION;
}
