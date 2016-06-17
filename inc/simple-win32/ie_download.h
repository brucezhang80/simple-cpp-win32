#ifndef IE_DOWNLOAD_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define IE_DOWNLOAD_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include <map>
#include <string>
#include <sstream>

#include <mshtml.h>

//
//	URLDownloadToFile
//
//	IE_DownloadCallbacker	callbacker;
//	callbacker.set_custom_header(L"App-Name", L"My Application");
//	if(SUCCEEDED(URLDownloadToFile(NULL, "http://127.0.0.1:8080/", "a.txt", 0, &callbacker))
//	{
//		...
//	}
//
class IE_DownloadCallbacker : public IBindStatusCallback, public IHttpNegotiate {
public:
    IE_DownloadCallbacker();

public:
    const std::wstring&		uri();
    HRESULT					result();

public:
    IBinding*				get_IBinding();
    long					binding_priority();
    void					set_binding_priority(long priority);
    DWORD					binding_flags();
    void					set_binding_flags(DWORD flags);
    int						binding_verb();
    void					set_binding_verb(int verb);
    void					set_binding_post_data(const char* data, size_t size);

    void					set_custom_header(const std::wstring& key, const std::wstring& value);
    std::wstring			get_custom_header(const std::wstring& key);
    void					clear_custom_headers();

public:
    // IBindStatusCallback methods.
    virtual HRESULT STDMETHODCALLTYPE OnStartBinding(
        /* [in] */ DWORD dwReserved,
        /* [in] */ IBinding *pib);
    virtual HRESULT STDMETHODCALLTYPE GetPriority(
        /* [out] */ LONG *pnPriority);
    virtual HRESULT STDMETHODCALLTYPE OnLowResource(
        /* [in] */ DWORD reserved);
    virtual HRESULT STDMETHODCALLTYPE OnProgress(
        /* [in] */ ULONG ulProgress,
        /* [in] */ ULONG ulProgressMax,
        /* [in] */ ULONG ulStatusCode,
        /* [in] */ LPCWSTR szStatusText);
    virtual HRESULT STDMETHODCALLTYPE OnStopBinding(
        /* [in] */ HRESULT hresult,
        /* [unique][in] */ LPCWSTR szError);
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetBindInfo(
        /* [out] */ DWORD *pgrfBINDF,
        /* [unique][out][in] */ BINDINFO *pbindinfo);
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE OnDataAvailable(
        /* [in] */ DWORD grfBSCF,
        /* [in] */ DWORD dwSize,
        /* [in] */ FORMATETC *pformatetc,
        /* [in] */ STGMEDIUM *pstgmed);
    virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(
        /* [in] */ REFIID riid,
        /* [iid_is][in] */ IUnknown *punk);

public:
    // IHttpNegotiate methods
    virtual HRESULT STDMETHODCALLTYPE BeginningTransaction(
        /* [in] */ LPCWSTR szURL,
        /* [unique][in] */ LPCWSTR szHeaders,
        /* [in] */ DWORD dwReserved,
        /* [out] */ LPWSTR *pszAdditionalHeaders);
    virtual HRESULT STDMETHODCALLTYPE OnResponse(
        /* [in] */ DWORD dwResponseCode,
        /* [unique][in] */ LPCWSTR szResponseHeaders,
        /* [unique][in] */ LPCWSTR szRequestHeaders,
        /* [out] */ LPWSTR *pszAdditionalRequestHeaders);

public:
    // IUnknown methods.
    // @Note that IE never calls any of these methods, since the caller owns the IBindStatusCallback interface
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

private:
    HRESULT									result_;
    IBinding*								binding_;
    int										binding_verb_;
    const char*								binding_post_data_;
    size_t									binding_post_data_size_;
    DWORD									binding_flags_;
    long									binding_priority_;
    std::map<std::wstring, std::wstring>	custom_headers;
    std::wstring							resource_url_;
};

//
//	URLOpenStream
//
//	IE_DownloadContentCallbacker	callback;
//	if( SUCCEEDED(URLOpenStream(NULL, "http://127.0.0.1:8080/?callback=asdf", 0, &callback))
//	{
//		...
//	}
//
class	IE_DownloadContentCallbacker	: public IE_DownloadCallbacker {
public:
    std::string			content() {
        return	ss_.str();
    }
    std::stringstream&	stream() {
        return	ss_;
    }

public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE OnDataAvailable(
        /* [in] */ DWORD grfBSCF,
        /* [in] */ DWORD dwSize,
        /* [in] */ FORMATETC *pformatetc,
        /* [in] */ STGMEDIUM *pstgmed);

protected:
    std::stringstream	ss_;
};

#endif
