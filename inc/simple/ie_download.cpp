
#include	"ie_download.h"
#include	<wininet.h>

IEDownload_Callbacker::IEDownload_Callbacker()
    : binding_(NULL)
    , binding_priority_(THREAD_PRIORITY_NORMAL)
    , binding_flags_(BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE |	BINDF_GETNEWESTVERSION | BINDF_RESYNCHRONIZE | BINDF_NOWRITECACHE)
    , binding_verb_(BINDVERB_GET)
    , binding_post_data_(NULL)
    , binding_post_data_size_(0)
    , result_(S_OK) {
}

HRESULT	IEDownload_Callbacker::result() {
    return	result_;
}

const std::wstring&	IEDownload_Callbacker::uri() {
    return	resource_url_;
}

IBinding*	IEDownload_Callbacker::get_IBinding() {
    return	binding_;
}

long	IEDownload_Callbacker::binding_priority() {
    return	binding_priority_;
}

void	IEDownload_Callbacker::set_binding_priority(long priority) {
    binding_priority_	= priority;
}

DWORD	IEDownload_Callbacker::binding_flags() {
    return	binding_flags_;
}

void	IEDownload_Callbacker::set_binding_flags(DWORD flags) {
    binding_flags_	= flags;
}

int		IEDownload_Callbacker::binding_verb() {
    return	binding_verb_;
}

void	IEDownload_Callbacker::set_binding_verb(int verb) {
    binding_verb_	= verb;
}

void	IEDownload_Callbacker::set_binding_post_data(const char* data, size_t size) {
    binding_verb_	= BINDVERB_POST;
    binding_post_data_		= data;
    binding_post_data_size_	= size;
}


void	IEDownload_Callbacker::set_custom_header(const std::wstring& key, const std::wstring& value) {
    if(!key.empty() && !value.empty()) {
        custom_headers[key]	= value;
    }
}

std::wstring	IEDownload_Callbacker::get_custom_header(const std::wstring& key) {
    return	custom_headers[key];
}

void	IEDownload_Callbacker::clear_custom_headers() {
    custom_headers.clear();
}

// IBindStatusCallback methods.
HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::OnStartBinding(
    /* [in] */ DWORD dwReserved,
    /* [in] */ IBinding *pib) {
    binding_	= pib;
    pib->AddRef();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::GetPriority(
    /* [out] */ LONG *pnPriority) {
    HRESULT hr = S_OK;
    if (NULL != pnPriority)
        *pnPriority = binding_priority_;
    else
        hr = E_INVALIDARG;
    return hr;
}

HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::OnLowResource(
    /* [in] */ DWORD reserved) {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::OnProgress(
    /* [in] */ ULONG ulProgress,
    /* [in] */ ULONG ulProgressMax,
    /* [in] */ ULONG ulStatusCode,
    /* [in] */ LPCWSTR szStatusText) {
    switch(ulStatusCode) {
    case BINDSTATUS_REDIRECTING: {
        if(0 != (BINDF_GETNEWESTVERSION & binding_flags_)) {
            DeleteUrlCacheEntryW(szStatusText);
        }
    }
    break;
    }

    switch(ulStatusCode) {
    case BINDSTATUS_REDIRECTING:
    case BINDSTATUS_BEGINDOWNLOADDATA:
    case BINDSTATUS_ENDDOWNLOADDATA:
    case BINDSTATUS_BEGINDOWNLOADCOMPONENTS:
    case BINDSTATUS_ENDDOWNLOADCOMPONENTS: {
        if(		NULL != szStatusText
                &&	0 != _wcsicmp(resource_url_.c_str(), szStatusText)
          ) {
            resource_url_	= szStatusText;
        }
    }
    break;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::OnStopBinding(
    /* [in] */ HRESULT hresult,
    /* [unique][in] */ LPCWSTR szError) {
    if(NULL != binding_) {
        binding_->Release();
        binding_	= NULL;
    }

    result_	= hresult;
    return S_OK;
}

/* [local] */ HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::GetBindInfo(
    /* [out] */ DWORD *pgrfBINDF,
    /* [unique][out][in] */ BINDINFO *pbindinfo) {
    if (pbindinfo==NULL || pbindinfo->cbSize==0 || pgrfBINDF==NULL) {
        return E_INVALIDARG;
    }

    *pgrfBINDF = binding_flags_;

    ULONG cbSize = pbindinfo->cbSize;		// remember incoming cbSize
    memset(pbindinfo, 0, cbSize);			// zero out structure
    pbindinfo->cbSize = cbSize;				// restore cbSize
    pbindinfo->dwBindVerb = binding_verb_;	// set verb
    if(BINDVERB_POST == binding_verb_ && binding_post_data_size_ > 0) {
        pbindinfo->stgmedData.hGlobal	= GlobalAlloc(GHND, binding_post_data_size_);
        if(NULL != pbindinfo->stgmedData.hGlobal) {
            pbindinfo->cbstgmedData			= binding_post_data_size_;
            pbindinfo->stgmedData.tymed		= TYMED_HGLOBAL;
            void*	pData	= GlobalLock(pbindinfo->stgmedData.hGlobal);
            memcpy(pData, binding_post_data_, binding_post_data_size_);
            GlobalUnlock(pbindinfo->stgmedData.hGlobal);
        }
    }
    return S_OK;
}

/* [local] */ HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::OnDataAvailable(
    /* [in] */ DWORD grfBSCF,
    /* [in] */ DWORD dwSize,
    /* [in] */ FORMATETC *pformatetc,
    /* [in] */ STGMEDIUM *pstgmed) {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::OnObjectAvailable(
    /* [in] */ REFIID riid,
    /* [iid_is][in] */ IUnknown *punk) {
    return S_OK;
}

// IHttpNegotiate methods
HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::BeginningTransaction(
    /* [in] */ LPCWSTR szURL,
    /* [unique][in] */ LPCWSTR szHeaders,
    /* [in] */ DWORD dwReserved,
    /* [out] */ LPWSTR *pszAdditionalHeaders) {
    // Here's opportunity to custom headers
    if (!pszAdditionalHeaders) {
        return E_POINTER;
    }

    *pszAdditionalHeaders = NULL;
    if(!custom_headers.empty()) {
        // 自定义HTTP头
        std::wstringstream	ss;
        std::map<std::wstring, std::wstring>::iterator	it		= custom_headers.begin();
        std::map<std::wstring, std::wstring>::iterator	it_end	= custom_headers.end();
        for(; it != it_end; ++it) {
            ss	<< it->first << L": " << it->second << L"\r\n";
        }

        size_t	size	= ss.str().size();
        LPWSTR wszAdditionalHeaders =  (LPWSTR)CoTaskMemAlloc((size + 1) *sizeof(WCHAR));
        if (!wszAdditionalHeaders) {
            return E_OUTOFMEMORY;
        }
        wcsncpy(wszAdditionalHeaders, ss.str().c_str(), size);
        wszAdditionalHeaders[size]	= 0;
        *pszAdditionalHeaders = wszAdditionalHeaders;
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::OnResponse(
    /* [in] */ DWORD dwResponseCode,
    /* [unique][in] */ LPCWSTR szResponseHeaders,
    /* [unique][in] */ LPCWSTR szRequestHeaders,
    /* [out] */ LPWSTR *pszAdditionalRequestHeaders) {
    return E_NOTIMPL;
}

// IUnknown methods.
// @Note that IE never calls any of these methods, since the caller owns the IBindStatusCallback interface
HRESULT STDMETHODCALLTYPE IEDownload_Callbacker::QueryInterface(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) {
    *ppvObject = NULL;

    if(riid == IID_IUnknown || riid == IID_IBindStatusCallback) {
        *ppvObject = (IBindStatusCallback*)this;
        AddRef();
        return S_OK;
    } else if(riid == IID_IHttpNegotiate) {
        *ppvObject = (IHttpNegotiate*)this;
        AddRef();
        return S_OK;
    } else {
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE IEDownload_Callbacker::AddRef(void) {
    return	1;
}

ULONG STDMETHODCALLTYPE IEDownload_Callbacker::Release(void) {
    return	1;
}

HRESULT STDMETHODCALLTYPE IEDownload_ContentCallbacker::OnDataAvailable(
    /* [in] */ DWORD grfBSCF,
    /* [in] */ DWORD dwSize,
    /* [in] */ FORMATETC *pformatetc,
    /* [in] */ STGMEDIUM *pstgmed) {
    IStream*	pStream	= pstgmed->pstm;

    char	buffer[1024]	= {0};
    ULONG	dwReaded		= 0;
    while(dwReaded < dwSize) {
        ULONG	dwCurrReaded;
        pStream->Read(buffer, sizeof(buffer), &dwCurrReaded);
        ss_.write(buffer, dwCurrReaded);
        dwReaded	+= dwCurrReaded;
    }
    return S_OK;
}
