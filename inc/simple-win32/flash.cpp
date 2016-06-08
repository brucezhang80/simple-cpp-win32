
#include "flash.h"

//////////////////////////////////////////////////////////////////////////
// Interfaces imported from flash.ocx

#include <assert.h>
#include <comdef.h>

#pragma pack(push, 8)

class FlashMemoryStream	: IStream {
public:
    FlashMemoryStream(void*	data,ULONG size) {
        this->data = data;
        this->size = size;
        this->pos =	0;
    }
    HRESULT	STDMETHODCALLTYPE QueryInterface(REFIID	riid, LPVOID* ppv) {
        return E_NOTIMPL;
    }
    ULONG STDMETHODCALLTYPE	AddRef() {
        return E_NOTIMPL;
    }
    ULONG STDMETHODCALLTYPE	Release() {
        return E_NOTIMPL;
    }
    // IStream methods
    STDMETHOD(Read)	(void *pv,ULONG	cb,ULONG *pcbRead) {
        if(pos == 0	&& cb == 4) {
            memcpy(pv,"fUfU",4);
            pos	+= 4;
            return S_OK;
        } else if(pos	== 4 &&	cb == 4) {
            memcpy(pv,&size,4);
            size +=	8;
            pos	+= 4;
            return S_OK;
        } else {
            if(pos + cb	> size)	cb = size -	pos;
            if(cb == 0)	return S_FALSE;
            memcpy(pv,(char*)data +	pos	- 8,cb);
            if(pcbRead)	(*pcbRead) = cb;
            pos	+= cb;
            return S_OK;
        }
    }
    STDMETHOD(Write) (void const *pv,ULONG cb,ULONG	*pcbWritten) {
        return E_NOTIMPL;
    }
    STDMETHOD(Seek)	(LARGE_INTEGER dlibMove,DWORD dwOrigin,ULARGE_INTEGER *plibNewPosition) {
        return E_NOTIMPL;
    }
    STDMETHOD(SetSize) (ULARGE_INTEGER libNewSize) {
        return	E_NOTIMPL;
    }
    STDMETHOD(CopyTo) (IStream *pstm,ULARGE_INTEGER	cb,ULARGE_INTEGER *pcbRead,ULARGE_INTEGER *pcbWritten) {
        return	E_NOTIMPL;
    }
    STDMETHOD(Commit) (DWORD grfCommitFlags) {
        return E_NOTIMPL;
    }
    STDMETHOD(Revert) (void) {
        return E_NOTIMPL;
    }
    STDMETHOD(LockRegion) (ULARGE_INTEGER libOffset,ULARGE_INTEGER cb,DWORD	dwLockType) {
        return E_NOTIMPL;
    }
    STDMETHOD(UnlockRegion)	(ULARGE_INTEGER	libOffset,ULARGE_INTEGER cb,DWORD dwLockType) {
        return E_NOTIMPL;
    }
    STDMETHOD(Stat)	(STATSTG *pstatstg,DWORD grfStatFlag) {
        return E_NOTIMPL;
    }
    STDMETHOD(Clone) (IStream **ppstm) {
        return	E_NOTIMPL;
    }
    void* data;
    ULONG size;
    ULONG pos;
};

namespace ShockwaveFlashObjects {

//
// go_forward references and typedefs
//

struct __declspec(uuid("d27cdb6b-ae6d-11cf-96b8-444553540000"))
/* LIBID */ __ShockwaveFlashObjects;
struct __declspec(uuid("d27cdb6c-ae6d-11cf-96b8-444553540000"))
/* dual interface */ IShockwaveFlash;
struct __declspec(uuid("d27cdb6d-ae6d-11cf-96b8-444553540000"))
/* dispinterface */ _IShockwaveFlashEvents;
struct /* coclass */ ShockwaveFlash;
struct /* coclass */ FlashProp;
struct __declspec(uuid("d27cdb70-ae6d-11cf-96b8-444553540000"))
/* interface */ IFlashFactory;
struct __declspec(uuid("d27cdb72-ae6d-11cf-96b8-444553540000"))
/* interface */ IFlashObjectInterface;
struct __declspec(uuid("a6ef9860-c720-11d0-9337-00a0c90dcaa9"))
/* interface */ IDispatchEx;
struct /* coclass */ FlashObjectInterface;

//
// Smart pointer typedef declarations
//

_COM_SMARTPTR_TYPEDEF(IShockwaveFlash, __uuidof(IShockwaveFlash));
_COM_SMARTPTR_TYPEDEF(_IShockwaveFlashEvents, __uuidof(_IShockwaveFlashEvents));
_COM_SMARTPTR_TYPEDEF(IFlashFactory, __uuidof(IFlashFactory));
_COM_SMARTPTR_TYPEDEF(IDispatchEx, __uuidof(IDispatchEx));
_COM_SMARTPTR_TYPEDEF(IFlashObjectInterface, __uuidof(IFlashObjectInterface));

//
// Type library items
//

struct __declspec(uuid("d27cdb6c-ae6d-11cf-96b8-444553540000"))
IShockwaveFlash : IDispatch {
    //
    // Property data
    //

    __declspec(property(get=GetStacking,put=PutStacking))
    _bstr_t Stacking;
    __declspec(property(get=GetWMode,put=PutWMode))
    _bstr_t WMode;
    __declspec(property(get=GetMovie,put=PutMovie))
    _bstr_t Movie;
    __declspec(property(get=GetSAlign,put=PutSAlign))
    _bstr_t SAlign;
    __declspec(property(get=GetMenu,put=PutMenu))
    VARIANT_BOOL Menu;
    __declspec(property(get=GetQuality,put=PutQuality))
    int Quality;
    __declspec(property(get=GetLoop,put=PutLoop))
    VARIANT_BOOL Loop;
    __declspec(property(get=GetFrameNum,put=PutFrameNum))
    long FrameNum;
    __declspec(property(get=GetBase,put=PutBase))
    _bstr_t Base;
    __declspec(property(get=Getscale,put=Putscale))
    _bstr_t scale;
    __declspec(property(get=GetDeviceFont,put=PutDeviceFont))
    VARIANT_BOOL DeviceFont;
    __declspec(property(get=GetEmbedMovie,put=PutEmbedMovie))
    VARIANT_BOOL EmbedMovie;
    __declspec(property(get=GetBGColor,put=PutBGColor))
    _bstr_t BGColor;
    __declspec(property(get=GetQuality2,put=PutQuality2))
    _bstr_t Quality2;
    __declspec(property(get=GetReadyState))
    long ReadyState;
    __declspec(property(get=GetScaleMode,put=PutScaleMode))
    int ScaleMode;
    __declspec(property(get=GetAlignMode,put=PutAlignMode))
    int AlignMode;
    __declspec(property(get=GetBackgroundColor,put=PutBackgroundColor))
    long BackgroundColor;
    __declspec(property(get=get_total_frames))
    long TotalFrames;
    __declspec(property(get=GetPlaying,put=PutPlaying))
    VARIANT_BOOL Playing;
    __declspec(property(get=GetSWRemote,put=PutSWRemote))
    _bstr_t SWRemote;

    //
    // Raw m_ethods provided by interface
    //

    virtual HRESULT __stdcall get_ReadyState (
        /*[out,retval]*/ long * thestate ) = 0;
    virtual HRESULT __stdcall get_TotalFrames (
        /*[out,retval]*/ long * numframes ) = 0;
    virtual HRESULT __stdcall get_Playing (
        /*[out,retval]*/ VARIANT_BOOL * Playing ) = 0;
    virtual HRESULT __stdcall put_Playing (
        /*[in]*/ VARIANT_BOOL Playing ) = 0;
    virtual HRESULT __stdcall get_Quality (
        /*[out,retval]*/ int * Quality ) = 0;
    virtual HRESULT __stdcall put_Quality (
        /*[in]*/ int Quality ) = 0;
    virtual HRESULT __stdcall get_ScaleMode (
        /*[out,retval]*/ int * scale ) = 0;
    virtual HRESULT __stdcall put_ScaleMode (
        /*[in]*/ int scale ) = 0;
    virtual HRESULT __stdcall get_AlignMode (
        /*[out,retval]*/ int * align ) = 0;
    virtual HRESULT __stdcall put_AlignMode (
        /*[in]*/ int align ) = 0;
    virtual HRESULT __stdcall get_BackgroundColor (
        /*[out,retval]*/ long * color ) = 0;
    virtual HRESULT __stdcall put_BackgroundColor (
        /*[in]*/ long color ) = 0;
    virtual HRESULT __stdcall get_Loop (
        /*[out,retval]*/ VARIANT_BOOL * Loop ) = 0;
    virtual HRESULT __stdcall put_Loop (
        /*[in]*/ VARIANT_BOOL Loop ) = 0;
    virtual HRESULT __stdcall get_Movie (
        /*[out,retval]*/ BSTR * path ) = 0;
    virtual HRESULT __stdcall put_Movie (
        /*[in]*/ BSTR path ) = 0;
    virtual HRESULT __stdcall get_FrameNum (
        /*[out,retval]*/ long * FrameNum ) = 0;
    virtual HRESULT __stdcall put_FrameNum (
        /*[in]*/ long FrameNum ) = 0;
    virtual HRESULT __stdcall SetZoomRect (
        /*[in]*/ long left,
        /*[in]*/ long top,
        /*[in]*/ long right,
        /*[in]*/ long bottom ) = 0;
    virtual HRESULT __stdcall Zoom (
        /*[in]*/ int factor ) = 0;
    virtual HRESULT __stdcall Pan (
        /*[in]*/ long x,
        /*[in]*/ long y,
        /*[in]*/ int m_ode ) = 0;
    virtual HRESULT __stdcall play ( ) = 0;
    virtual HRESULT __stdcall Stop ( ) = 0;
    virtual HRESULT __stdcall go_back ( ) = 0;
    virtual HRESULT __stdcall go_forward ( ) = 0;
    virtual HRESULT __stdcall go_rewind ( ) = 0;
    virtual HRESULT __stdcall StopPlay ( ) = 0;
    virtual HRESULT __stdcall goto_frame (
        /*[in]*/ long FrameNum ) = 0;
    virtual HRESULT __stdcall CurrentFrame (
        /*[out,retval]*/ long * FrameNum ) = 0;
    virtual HRESULT __stdcall is_playing (
        /*[out,retval]*/ VARIANT_BOOL * Playing ) = 0;
    virtual HRESULT __stdcall PercentLoaded (
        /*[out,retval]*/ long * __MIDL_0011 ) = 0;
    virtual HRESULT __stdcall FrameLoaded (
        /*[in]*/ long FrameNum,
        /*[out,retval]*/ VARIANT_BOOL * loaded ) = 0;
    virtual HRESULT __stdcall FlashVersion (
        /*[out,retval]*/ long * version ) = 0;
    virtual HRESULT __stdcall get_WMode (
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall put_WMode (
        /*[in]*/ BSTR pVal ) = 0;
    virtual HRESULT __stdcall get_SAlign (
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall put_SAlign (
        /*[in]*/ BSTR pVal ) = 0;
    virtual HRESULT __stdcall get_Menu (
        /*[out,retval]*/ VARIANT_BOOL * pVal ) = 0;
    virtual HRESULT __stdcall put_Menu (
        /*[in]*/ VARIANT_BOOL pVal ) = 0;
    virtual HRESULT __stdcall get_Base (
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall put_Base (
        /*[in]*/ BSTR pVal ) = 0;
    virtual HRESULT __stdcall get_scale (
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall put_scale (
        /*[in]*/ BSTR pVal ) = 0;
    virtual HRESULT __stdcall get_DeviceFont (
        /*[out,retval]*/ VARIANT_BOOL * pVal ) = 0;
    virtual HRESULT __stdcall put_DeviceFont (
        /*[in]*/ VARIANT_BOOL pVal ) = 0;
    virtual HRESULT __stdcall get_EmbedMovie (
        /*[out,retval]*/ VARIANT_BOOL * pVal ) = 0;
    virtual HRESULT __stdcall put_EmbedMovie (
        /*[in]*/ VARIANT_BOOL pVal ) = 0;
    virtual HRESULT __stdcall get_BGColor (
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall put_BGColor (
        /*[in]*/ BSTR pVal ) = 0;
    virtual HRESULT __stdcall get_Quality2 (
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall put_Quality2 (
        /*[in]*/ BSTR pVal ) = 0;
    virtual HRESULT __stdcall LoadMovie (
        /*[in]*/ int layer,
        /*[in]*/ BSTR url ) = 0;
    virtual HRESULT __stdcall TGotoFrame (
        /*[in]*/ BSTR target,
        /*[in]*/ long FrameNum ) = 0;
    virtual HRESULT __stdcall TGotoLabel (
        /*[in]*/ BSTR target,
        /*[in]*/ BSTR label ) = 0;
    virtual HRESULT __stdcall TCurrentFrame (
        /*[in]*/ BSTR target,
        /*[out,retval]*/ long * FrameNum ) = 0;
    virtual HRESULT __stdcall TCurrentLabel (
        /*[in]*/ BSTR target,
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall TPlay (
        /*[in]*/ BSTR target ) = 0;
    virtual HRESULT __stdcall TStopPlay (
        /*[in]*/ BSTR target ) = 0;
    virtual HRESULT __stdcall set_variable (
        /*[in]*/ BSTR name,
        /*[in]*/ BSTR value ) = 0;
    virtual HRESULT __stdcall get_variable (
        /*[in]*/ BSTR name,
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall TSetProperty (
        /*[in]*/ BSTR target,
        /*[in]*/ int property,
        /*[in]*/ BSTR value ) = 0;
    virtual HRESULT __stdcall TGetProperty (
        /*[in]*/ BSTR target,
        /*[in]*/ int property,
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall TCallFrame (
        /*[in]*/ BSTR target,
        /*[in]*/ int FrameNum ) = 0;
    virtual HRESULT __stdcall TCallLabel (
        /*[in]*/ BSTR target,
        /*[in]*/ BSTR label ) = 0;
    virtual HRESULT __stdcall TSetPropertyNum (
        /*[in]*/ BSTR target,
        /*[in]*/ int property,
        /*[in]*/ double value ) = 0;
    virtual HRESULT __stdcall TGetPropertyNum (
        /*[in]*/ BSTR target,
        /*[in]*/ int property,
        /*[out,retval]*/ double * pVal ) = 0;
    virtual HRESULT __stdcall get_SWRemote (
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall put_SWRemote (
        /*[in]*/ BSTR pVal ) = 0;
    virtual HRESULT __stdcall get_Stacking (
        /*[out,retval]*/ BSTR * pVal ) = 0;
    virtual HRESULT __stdcall put_Stacking (
        /*[in]*/ BSTR pVal ) = 0;
};

struct __declspec(uuid("d27cdb6d-ae6d-11cf-96b8-444553540000"))
_IShockwaveFlashEvents : IDispatch {
    //
    // Wrapper m_ethods for error-handling
    //

    // m_ethods:
    HRESULT OnReadyStateChange (
        long newState );
    HRESULT OnProgress (
        long percentDone );
    HRESULT FSCommand (
        _bstr_t command,
        _bstr_t args );
};

struct __declspec(uuid("d27cdb6e-ae6d-11cf-96b8-444553540000"))
ShockwaveFlash;
// [ default ] interface IShockwaveFlash
// [ default, source ] dispinterface _IShockwaveFlashEvents

struct __declspec(uuid("1171a62f-05d2-11d1-83fc-00a0c9089c5a"))
FlashProp;
// [ default ] interface IUnknown

struct __declspec(uuid("d27cdb70-ae6d-11cf-96b8-444553540000"))
IFlashFactory : IUnknown {
};

struct __declspec(uuid("a6ef9860-c720-11d0-9337-00a0c90dcaa9"))
IDispatchEx : IDispatch {
    //
    // Wrapper m_ethods for error-handling
    //

    HRESULT GetDispID (
        _bstr_t bstrName,
        unsigned long grfdex,
        long * pid );
    HRESULT RemoteInvokeEx (
        long id,
        unsigned long lcid,
        unsigned long dwFlags,
        struct DISPPARAMS * pdp,
        VARIANT * pvarRes,
        struct EXCEPINFO * pei,
        struct IServiceProvider * pspCaller,
        unsigned int cvarRefArg,
        unsigned int * rgiRefArg,
        VARIANT * rgvarRefArg );
    HRESULT DeleteMemberByName (
        _bstr_t bstrName,
        unsigned long grfdex );
    HRESULT DeleteMemberByDispID (
        long id );
    HRESULT GetMemberProperties (
        long id,
        unsigned long grfdexFetch,
        unsigned long * pgrfdex );
    HRESULT GetMemberName (
        long id,
        BSTR * pbstrName );
    HRESULT GetNextDispID (
        unsigned long grfdex,
        long id,
        long * pid );
    HRESULT GetNameSpaceParent (
        IUnknown * * ppunk );

    //
    // Raw m_ethods provided by interface
    //

    virtual HRESULT __stdcall raw_GetDispID (
        /*[in]*/ BSTR bstrName,
        /*[in]*/ unsigned long grfdex,
        /*[out]*/ long * pid ) = 0;
    virtual HRESULT __stdcall raw_RemoteInvokeEx (
        /*[in]*/ long id,
        /*[in]*/ unsigned long lcid,
        /*[in]*/ unsigned long dwFlags,
        /*[in]*/ struct DISPPARAMS * pdp,
        /*[out]*/ VARIANT * pvarRes,
        /*[out]*/ struct EXCEPINFO * pei,
        /*[in]*/ struct IServiceProvider * pspCaller,
        /*[in]*/ unsigned int cvarRefArg,
        /*[in]*/ unsigned int * rgiRefArg,
        /*[in,out]*/ VARIANT * rgvarRefArg ) = 0;
    virtual HRESULT __stdcall raw_DeleteMemberByName (
        /*[in]*/ BSTR bstrName,
        /*[in]*/ unsigned long grfdex ) = 0;
    virtual HRESULT __stdcall raw_DeleteMemberByDispID (
        /*[in]*/ long id ) = 0;
    virtual HRESULT __stdcall raw_GetMemberProperties (
        /*[in]*/ long id,
        /*[in]*/ unsigned long grfdexFetch,
        /*[out]*/ unsigned long * pgrfdex ) = 0;
    virtual HRESULT __stdcall raw_GetMemberName (
        /*[in]*/ long id,
        /*[out]*/ BSTR * pbstrName ) = 0;
    virtual HRESULT __stdcall raw_GetNextDispID (
        /*[in]*/ unsigned long grfdex,
        /*[in]*/ long id,
        /*[out]*/ long * pid ) = 0;
    virtual HRESULT __stdcall raw_GetNameSpaceParent (
        /*[out]*/ IUnknown * * ppunk ) = 0;
};

struct __declspec(uuid("d27cdb72-ae6d-11cf-96b8-444553540000"))
IFlashObjectInterface : IDispatchEx {
};

struct __declspec(uuid("d27cdb71-ae6d-11cf-96b8-444553540000"))
FlashObjectInterface;
// [ default ] interface IFlashObjectInterface

} // namespace ShockwaveFlashObjects

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
// FlashSink - Receives flash events

class FlashSink : public ShockwaveFlashObjects::_IShockwaveFlashEvents {
public:
    LPCONNECTIONPOINT		m_ConnectionPoint;
    DWORD					m_Cookie;
    int						m_RefCount;
    Flash_Player*			m_FlashWidget;

public:
    FlashSink() {
        m_Cookie = 0;
        m_ConnectionPoint = NULL;
        m_RefCount = 0;
        m_FlashWidget = NULL;
    }

    virtual ~FlashSink() {
        m_FlashWidget->m_COMCount--;
    }

    HRESULT Init(Flash_Player* theFlashWidget) {
        m_FlashWidget = theFlashWidget;
        m_FlashWidget->m_COMCount++;

        HRESULT aResult = NOERROR;
        LPCONNECTIONPOINTCONTAINER aConnectionPoint = NULL;

        if ((m_FlashWidget->m_FlashInterface->QueryInterface(IID_IConnectionPointContainer, (void**) &aConnectionPoint) == S_OK) &&
                (aConnectionPoint->FindConnectionPoint(__uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents), &m_ConnectionPoint) == S_OK)) {
            IDispatch* aDispatch = NULL;
            QueryInterface(__uuidof(IDispatch), (void**) &aDispatch);
            if (aDispatch != NULL) {
                aResult = m_ConnectionPoint->Advise((LPUNKNOWN)aDispatch, &m_Cookie);
                aDispatch->Release();
            }
        }

        if (aConnectionPoint != NULL)
            aConnectionPoint->Release();

        return aResult;
    }

    HRESULT Shutdown() {
        HRESULT aResult = S_OK;

        if (m_ConnectionPoint) {
            if (m_Cookie) {
                aResult = m_ConnectionPoint->Unadvise(m_Cookie);
                m_Cookie = 0;
            }

            m_ConnectionPoint->Release();
            m_ConnectionPoint = NULL;
        }

        return aResult;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv) {
        *ppv = NULL;

        if (riid == IID_IUnknown) {
            *ppv = (LPUNKNOWN)this;
            AddRef();
            return S_OK;
        } else if (riid == IID_IDispatch) {
            *ppv = (IDispatch*)this;
            AddRef();
            return S_OK;
        } else if (riid == __uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents)) {
            *ppv = (ShockwaveFlashObjects::_IShockwaveFlashEvents*) this;
            AddRef();
            return S_OK;
        } else {
            return E_NOTIMPL;
        }
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        return ++m_RefCount;
    }

    ULONG STDMETHODCALLTYPE Release() {
        int aRefCount = --m_RefCount;
        if (aRefCount == 0)
            delete this;

        return aRefCount;
    }

// IDispatch m_ethod
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames,
            UINT cNames, LCID lcid,DISPID* rgDispId) {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
            WORD wFlags, ::DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult,
            ::EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr) {
        switch(dispIdMember) {
        case 0x7a6:
            break;
        case 0x96:
            if ((pDispParams->cArgs == 2) &&
                    (pDispParams->rgvarg[0].vt == VT_BSTR) &&
                    (pDispParams->rgvarg[1].vt == VT_BSTR)) {
                FSCommand(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
            }
            break;
        case DISPID_READYSTATECHANGE:
            break;
        default:
            return DISP_E_MEMBERNOTFOUND;
        }

        return NOERROR;
    }

    HRESULT OnReadyStateChange (long newState) {
        return S_OK;
    }

    HRESULT OnProgress(long percentDone ) {
        return S_OK;
    }

    HRESULT FSCommand (_bstr_t command, _bstr_t args) {
        m_FlashWidget->do_on_flash_command((char*) command, (char*) args);
        return S_OK;
    }
};

using namespace ShockwaveFlashObjects;

//////////////////////////////////////////////////////////////////////////
// FlashControlSite - container that holds the flash control

class FlashControlSite :
    public IOleInPlaceSiteWindowless,
    public IOleClientSite {
public:
    int						m_RefCount;
    Flash_Player*			m_FlashWidget;

public:
    FlashControlSite() {
        m_RefCount = 0;
        m_FlashWidget = NULL;
    }

    virtual ~FlashControlSite() {
        if (m_FlashWidget != NULL)
            m_FlashWidget->m_COMCount--;
    }

    void Init(Flash_Player* theFlashWidget) {
        m_FlashWidget = theFlashWidget;
        m_FlashWidget->m_COMCount++;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv) {
        *ppv = NULL;

        if (riid == IID_IUnknown) {
            *ppv = (IUnknown*) (IOleWindow*) this;
            AddRef();
            return S_OK;
        } else if (riid == IID_IOleWindow) {
            *ppv = (IOleWindow*)this;
            AddRef();
            return S_OK;
        } else if (riid == IID_IOleInPlaceSite) {
            *ppv = (IOleInPlaceSite*)this;
            AddRef();
            return S_OK;
        } else if (riid == IID_IOleInPlaceSiteEx) {
            *ppv = (IOleInPlaceSiteEx*)this;
            AddRef();
            return S_OK;
        } else if (riid == IID_IOleInPlaceSiteWindowless) {
            *ppv = (IOleInPlaceSiteWindowless*)this;
            AddRef();
            return S_OK;
        } else if (riid == IID_IOleClientSite) {
            *ppv = (IOleClientSite*)this;
            AddRef();
            return S_OK;
        } else if (riid == __uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents)) {
            *ppv = (ShockwaveFlashObjects::_IShockwaveFlashEvents*) this;
            AddRef();
            return S_OK;
        } else {
            return E_NOTIMPL;
        }
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        return ++m_RefCount;
    }

    ULONG STDMETHODCALLTYPE Release() {
        int aRefCount = --m_RefCount;
        if (aRefCount == 0)
            delete this;

        return aRefCount;
    }

    //////////////////////////////////////////////////////////////////////////

    virtual HRESULT  STDMETHODCALLTYPE SaveObject(void) {
        return S_OK;
    }

    virtual HRESULT  STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,IMoniker** ppmk ) {
        *ppmk = NULL;
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer ** theContainerP) {
        //return QueryInterface(__uuidof(IOleContainer), (void**) theContainerP);
        return E_NOINTERFACE;
    }


    virtual HRESULT  STDMETHODCALLTYPE ShowObject(void) {
        return E_NOTIMPL;
    }

    virtual HRESULT  STDMETHODCALLTYPE OnShowWindow(BOOL) {
        return E_NOTIMPL;
    }

    virtual HRESULT  STDMETHODCALLTYPE RequestNewObjectLayout(void) {
        return E_NOTIMPL;
    }
    //


    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(/* [in] */ BOOL fEnterMode) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetWindow(/* [out] */ HWND __RPC_FAR* theWnndow) {
        return E_FAIL;

        //*theWnndow = NULL;
        //*theWnndow = gSexyAppBase->m_HWnd;
        //return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnUIActivate(void) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE GetWindowContext(/* [out] */ IOleInPlaceFrame __RPC_FAR *__RPC_FAR *ppFrame, /* [out] */ IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR *ppDoc, /* [out] */ LPRECT lprcPosRect, /* [out] */ LPRECT lprcClipRect, /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo) {
        RECT aRect;
        SetRect(&aRect, 0, 0, m_FlashWidget->m_nWidth, m_FlashWidget->m_nHeight);

        *lprcPosRect = aRect;
        *lprcClipRect = aRect;

        *ppFrame = NULL;
        QueryInterface(__uuidof(IOleInPlaceFrame), (void**) ppFrame);
        *ppDoc = NULL;

        lpFrameInfo->fMDIApp = FALSE;
        lpFrameInfo->hwndFrame = NULL;
        lpFrameInfo->haccel = NULL;
        lpFrameInfo->cAccelEntries = 0;

        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE Scroll(/* [in] */ SIZE scrollExtant) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnUIDeactivate(/* [in] */ BOOL fUndoable) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE DiscardUndoState(void) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnPosRectChange(/* [in] */ LPCRECT lprcPosRect) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(/* [out] */ BOOL __RPC_FAR *pfNoRedraw, /* [in] */ DWORD dwFlags) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(/* [in] */ BOOL fNoRedraw) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE RequestUIActivate(void) {
        return S_FALSE;
    }


    HRESULT STDMETHODCALLTYPE CanWindowlessActivate(void) {
        // Allow windowless activation?
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE GetCapture(void) {
        // TODO capture the m_ouse for the object
        return S_FALSE;
    }


    HRESULT STDMETHODCALLTYPE SetCapture(/* [in] */ BOOL fCapture) {
        // TODO capture the m_ouse for the object
        return S_FALSE;
    }


    HRESULT STDMETHODCALLTYPE GetFocus(void) {
        return S_OK;
    }


    HRESULT STDMETHODCALLTYPE SetFocus(/* [in] */ BOOL fFocus) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetDC(/* [in] */ LPCRECT pRect, /* [in] */ DWORD grfFlags, /* [out] */ HDC __RPC_FAR *phDC) {
        return E_INVALIDARG;
    }


    HRESULT STDMETHODCALLTYPE ReleaseDC(/* [in] */ HDC hDC) {
        return E_INVALIDARG;
    }


    HRESULT STDMETHODCALLTYPE InvalidateRect(/* [in] */ LPCRECT pRect, /* [in] */ BOOL fErase) {
        if (pRect == NULL) {
            m_FlashWidget->m_FlashDirty = true;
            SetRect(&m_FlashWidget->m_DirtyRect, 0, 0, m_FlashWidget->m_nWidth, m_FlashWidget->m_nHeight);
        } else if (!m_FlashWidget->m_FlashDirty) {
            m_FlashWidget->m_FlashDirty = true;
            SetRect(&m_FlashWidget->m_DirtyRect, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top);
        } else {
            RECT aRect = m_FlashWidget->m_DirtyRect;
            UnionRect(&m_FlashWidget->m_DirtyRect, pRect, &aRect);
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE InvalidateRgn(/* [in] */ HRGN hRGN, /* [in] */ BOOL fErase) {
        m_FlashWidget->m_FlashDirty = true;
        SetRect(&m_FlashWidget->m_DirtyRect, 0, 0, m_FlashWidget->m_nWidth, m_FlashWidget->m_nHeight);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ScrollRect(/* [in] */ INT dx, /* [in] */ INT dy, /* [in] */ LPCRECT pRectScroll, /* [in] */ LPCRECT pRectClip) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AdjustRect(/* [out][in] */ LPRECT prc) {
        if (prc == NULL) {
            return E_INVALIDARG;
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDefWindowMessage(/* [in] */ UINT m_sg, /* [in] */ WPARAM wParam, /* [in] */ LPARAM lParam, /* [out] */ LRESULT __RPC_FAR *plResult) {
        return S_FALSE;
    }
};

//////////////////////////////////////////////////////////////////////////

const CLSID CLSID_ShockwaveFlash =
{ 0xD27CDB6E, 0xAE6D, 0x11cf, {0x96, 0xB8, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };

typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

Flash_Player::Flash_Player() {
    HRESULT aResult;

    m_State = STATE_IDLE;

    m_nWidth	= 0;
    m_nHeight	= 0;

    m_hDC		= NULL;
    m_hBmp		= NULL;
    m_pDIB		= NULL;

    m_bkColor	= RGB(1, 0, 1);

    m_COMCount		= 0;
    m_PauseCount	= 0;
    m_FlashDirty	= true;

    m_nTotalFrames	= -1;
    m_nCurrFrame	= -1;
    m_nLoadingPercent	= 0;

    m_FlashSink = NULL;
    m_FlashInterface = NULL;
    m_OleObject = NULL;
    m_WindowlessObject = NULL;

    m_FlashLibHandle = NULL;

    CoInitialize(NULL);

    m_ControlSite = new FlashControlSite();
    m_ControlSite->AddRef();
    m_ControlSite->Init(this);

    handler_load_flash_library.clear();
    DllGetClassObjectFunc aDllGetClassObjectFunc = NULL;
    if(handler_load_flash_library) {
        m_FlashLibHandle = handler_load_flash_library(this, reinterpret_cast<void**>(&aDllGetClassObjectFunc));
    }

    if(NULL == m_FlashLibHandle) {
        m_FlashLibHandle = LoadLibraryA("flash.ocx");
        if (m_FlashLibHandle != NULL) {
            aDllGetClassObjectFunc = (DllGetClassObjectFunc) GetProcAddress(m_FlashLibHandle, "DllGetClassObject");
        }
    }

    if(NULL != m_FlashLibHandle) {
        if(NULL != aDllGetClassObjectFunc) {
            IClassFactory* aClassFactory = NULL;
            aResult = aDllGetClassObjectFunc(CLSID_ShockwaveFlash, IID_IClassFactory, (void**)&aClassFactory);
            aClassFactory->CreateInstance(NULL, IID_IOleObject, (void**)&m_OleObject);
            aClassFactory->Release();
        } else {
            if(handler_unload_flash_library) {
                handler_unload_flash_library(this, m_FlashLibHandle);
            } else {
                FreeLibrary(m_FlashLibHandle);
                m_FlashLibHandle	= NULL;
            }
        }
    }

    if (NULL == m_OleObject) {
        CoCreateInstance(CLSID_ShockwaveFlash, NULL,
                         CLSCTX_INPROC_SERVER,
                         IID_IOleObject,
                         (void**)&m_OleObject);
    }

    IOleClientSite* aClientSite = NULL;
    m_ControlSite->QueryInterface(__uuidof(IOleClientSite), (void**) &aClientSite);
    m_OleObject->SetClientSite(aClientSite);

    m_OleObject->QueryInterface(__uuidof(IShockwaveFlash), (LPVOID*) &m_FlashInterface);
    _bstr_t aTrans = "Transparent";
    m_FlashInterface->put_WMode(aTrans);

    aResult = m_OleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, aClientSite, 0, NULL, NULL);
    aClientSite->Release();

    m_OleObject->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (LPVOID*) &m_WindowlessObject);

    m_FlashSink = new FlashSink();
    m_FlashSink->AddRef();
    m_FlashSink->Init(this);

    resize(400, 300);
}

Flash_Player::~Flash_Player() {
    do_cleanup_images();

    if (m_FlashSink != NULL) {
        m_FlashSink->Shutdown();
        m_FlashSink->Release();
    }

    if (m_WindowlessObject != NULL) {
        m_WindowlessObject->Release();
    }

    if (m_OleObject != NULL) {
        m_OleObject->Close(OLECLOSE_NOSAVE);
        m_OleObject->SetClientSite(NULL);
        m_OleObject->Release();
    }

    if (m_FlashInterface != NULL) {
        m_FlashInterface->Release();
    }

    if (m_ControlSite != NULL) {
        m_ControlSite->Release();
    }

    // m_ake sure all our COM objects were actually destroyed
    assert(0 == m_COMCount);

    if (m_FlashLibHandle != NULL) {
        if(handler_unload_flash_library) {
            handler_unload_flash_library(this, m_FlashLibHandle);
        } else {
            FreeLibrary(m_FlashLibHandle);
            m_FlashLibHandle	= NULL;
        }
    }

    CoUninitialize();
}

double Flash_Player::get_flash_version() {
    CoInitialize(NULL);

    IOleObject* anOleObject = NULL;
    if (FAILED(CoCreateInstance(CLSID_ShockwaveFlash, NULL,
                                CLSCTX_INPROC_SERVER,
                                IID_IOleObject,
                                (void**) &anOleObject))) {
        CoUninitialize();
        return 0.0;
    }

    IShockwaveFlash* aFlashInterface = NULL;
    if (FAILED(anOleObject->QueryInterface(__uuidof(IShockwaveFlash), (LPVOID*) &aFlashInterface))) {
        CoUninitialize();
        return 0.0;
    }

    long aVersion = 0;
    aFlashInterface->FlashVersion(&aVersion);

    aFlashInterface->Release();
    anOleObject->Release();

    CoUninitialize();
    return aVersion / 65536.0;
}

void Flash_Player::do_cleanup_images() {
    if (m_hBmp != NULL) {
        DeleteObject(m_hBmp);
    }

    if (m_hDC != NULL) {
        DeleteDC(m_hDC);
    }
}

void Flash_Player::do_rebuild_images() {
    set_dirty();
    do_cleanup_images();

    HDC desktop_dc = ::GetDC(NULL);
    m_hDC = ::CreateCompatibleDC(desktop_dc);
    ReleaseDC(NULL,desktop_dc);

    BITMAPINFO	bmpInfo;
    memset(&bmpInfo,0, sizeof(bmpInfo));
    bmpInfo.bmiHeader.biSize = sizeof(bmpInfo);
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biWidth	= m_nWidth;
    bmpInfo.bmiHeader.biHeight	= -m_nHeight;

    m_hBmp = CreateDIBSection(m_hDC, &bmpInfo, DIB_RGB_COLORS, &m_pDIB, NULL, 0);
    SelectObject(m_hDC, m_hBmp);

    ::SetMapMode(m_hDC, MM_TEXT);
    ::SetBkColor(m_hDC,RGB(255,255,255));
    ::SetBkMode(m_hDC,TRANSPARENT);
}

void Flash_Player::do_check_cursor() {
    // TODO: 更改光标形状
    //HCURSOR aCursor = GetCursor();
    //if (aCursor == ::LoadCursor(NULL, IDC_ARROW))
    //{
    //}
    //else if (aCursor != NULL)
    //{
    //}
}

bool Flash_Player::play(const std::string& theFileName) {
    std::string	aFullPath(theFileName);
    if(!theFileName.empty() && std::string::npos == theFileName.find(':')) {
        if(theFileName[0] != '\\') {
            aFullPath = "\\" + theFileName;
        }

        char buffer[MAX_PATH];
        ::GetCurrentDirectory(sizeof(buffer), buffer);
        aFullPath = buffer + aFullPath;
    }

    {
        _bstr_t bstr((char*) aFullPath.c_str());
        m_FlashInterface->put_Movie(bstr);
        if ((m_PauseCount == 0) && (m_FlashInterface->play() != S_OK)) {
            return false;
        }
    }

    do_rebuild_images();

    {
        IOleInPlaceObject* anInPlaceObject = NULL;
        m_OleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*) &anInPlaceObject);
        if (anInPlaceObject != NULL) {
            RECT rc	= {0, 0, m_nWidth, m_nHeight};
            anInPlaceObject->SetObjectRects(&rc, &rc);
            anInPlaceObject->Release();
        }
    }

    m_State = STATE_PLAYING;
    return true;
}

bool Flash_Player::play(void* data, size_t length) {
    {
        FlashMemoryStream fStream(data,	length);
        IPersistStreamInit*	psStreamInit = NULL;
        m_FlashInterface->QueryInterface(IID_IPersistStreamInit,(LPVOID*)	&psStreamInit);
        if(NULL == psStreamInit) {
            return	false;
        }

        psStreamInit->InitNew();
        psStreamInit->Load((LPSTREAM)&fStream);
        psStreamInit->Release();
    }

    do_rebuild_images();

    {
        IOleInPlaceObject* anInPlaceObject = NULL;
        m_OleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*) &anInPlaceObject);
        if (anInPlaceObject != NULL) {
            RECT rc	= {0, 0, m_nWidth, m_nHeight};
            anInPlaceObject->SetObjectRects(&rc, &rc);
            anInPlaceObject->Release();
        }
    }

    m_State = STATE_PLAYING;
    return true;
}

void Flash_Player::set_quality(int theQuality) {
    static char* aQualityNames[3] = {"Low", "m_edium", "High"};

    _bstr_t aNewStr = aQualityNames[theQuality];
    m_FlashInterface->put_Quality2(aNewStr);
}

void Flash_Player::pause() {
    m_PauseCount++;

    if (m_State != STATE_STOPPED) {
        m_State = STATE_IDLE;
    }

    if ((m_PauseCount == 1) && (m_FlashInterface != NULL) && (m_State != STATE_STOPPED)) {
        m_FlashInterface->StopPlay();
    }
}

void Flash_Player::resume() {
    m_PauseCount--;
    if ((m_PauseCount == 0) && (m_FlashInterface != NULL) && (m_State != STATE_STOPPED)) {
        m_State = STATE_PLAYING;
        m_FlashInterface->play();
    }
}

void Flash_Player::go_rewind() {
    if (m_FlashInterface != NULL) {
        m_FlashInterface->go_rewind();
        m_FlashInterface->play();
    }
}

void Flash_Player::goto_frame(int theFrameNum) {
    if (m_FlashInterface != NULL) {
        m_FlashInterface->goto_frame(theFrameNum);
        m_FlashInterface->play();
    }
}

void Flash_Player::go_back() {
    if (m_FlashInterface != NULL) {
        m_FlashInterface->go_back();
        m_FlashInterface->play();
    }
}

void Flash_Player::go_forward() {
    if (m_FlashInterface != NULL) {
        m_FlashInterface->go_forward();
        m_FlashInterface->play();
    }
}

bool Flash_Player::is_playing() {
    return (STATE_STOPPED != m_State);
}

int Flash_Player::get_current_frame() {
    return m_nCurrFrame;
}

std::string Flash_Player::get_current_label(const std::string& theTimeline) {
    BSTR aBStr = L"";
    if (m_FlashInterface != NULL)
        m_FlashInterface->TCurrentLabel(_bstr_t(theTimeline.c_str()), &aBStr);
    return (const char*) _bstr_t(aBStr);
}

void Flash_Player::call_frame(const std::string& theTimeline, int theFrameNum) {
    if (m_FlashInterface != NULL)
        m_FlashInterface->TCallFrame(_bstr_t(theTimeline.c_str()), theFrameNum);
}

void Flash_Player::call_label(const std::string& theTimeline, const std::string& theLabel) {
    if (m_FlashInterface != NULL)
        m_FlashInterface->TCallLabel(_bstr_t(theTimeline.c_str()), _bstr_t(theLabel.c_str()));
}

std::string Flash_Player::get_variable(const std::string& theName) {
    BSTR aBStr = L"";
    if (m_FlashInterface != NULL)
        m_FlashInterface->get_variable(_bstr_t(theName.c_str()), &aBStr);
    return (const char*) _bstr_t(aBStr);
}

void Flash_Player::set_variable(const std::string& theName, const std::string& theValue) {
    if (m_FlashInterface != NULL)
        m_FlashInterface->set_variable(_bstr_t(theName.c_str()), _bstr_t(theValue.c_str()));
}

void Flash_Player::update() {
    // 更新总帧数信息
    {
        m_nTotalFrames = -1;
        if (m_FlashInterface != NULL) {
            m_FlashInterface->get_TotalFrames(&m_nTotalFrames);
        }
    }

    // 更新及当前帧信息
    {
        m_nCurrFrame = -1;
        if (m_FlashInterface != NULL) {
            m_FlashInterface->CurrentFrame(&m_nCurrFrame);
        }
    }

    // 更新载入信息
    {
        m_nLoadingPercent = -1;
        if (m_FlashInterface != NULL) {
            m_FlashInterface->PercentLoaded(&m_nLoadingPercent);
        }
    }

    VARIANT_BOOL isPlaying = 0;
    m_FlashInterface->is_playing(&isPlaying);

    if(!isPlaying) {
        m_FlashInterface->get_Playing(&isPlaying);
    }

    if(isPlaying && m_State != STATE_PLAYING) {
        m_State = STATE_PLAYING;
    } else if (!isPlaying && m_State != STATE_STOPPED) {
        m_State = STATE_STOPPED;

        if (event_flash_stopped) {
            event_flash_stopped(this);
        }
    }
}

void Flash_Player::resize(int theWidth, int theHeight) {
    m_nWidth	= theWidth;
    m_nHeight	= theHeight;
    SetRect(&m_DirtyRect, 0, 0, m_nWidth, m_nHeight);

    do_rebuild_images();

    IOleInPlaceObject* anInPlaceObject = NULL;
    m_OleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*) &anInPlaceObject);

    if (anInPlaceObject != NULL) {
        RECT aRect = {0, 0, m_nWidth, m_nHeight};
        anInPlaceObject->SetObjectRects(&aRect, &aRect);
        anInPlaceObject->Release();
    }
}

void Flash_Player::draw(HDC hDC, bool bClear, int x, int y) {
    if(NULL == hDC) {
        hDC	= m_hDC;
    }

    // 清空背景颜色
    if(bClear) {
        RECT rcRect = {0, 0, m_nWidth, m_nHeight};
        ::SetBkColor	(hDC, m_bkColor);
        ::ExtTextOut	(hDC, 0, 0, ETO_OPAQUE, &rcRect, NULL, 0, NULL);
    }

    {
        IViewObject* aViewObject = NULL;
        m_FlashInterface->QueryInterface(IID_IViewObject, (LPVOID*) &aViewObject);
        if (aViewObject != NULL) {
            if(!bClear) {
                HRGN aRgn = CreateRectRgn(m_DirtyRect.left, m_DirtyRect.top, m_DirtyRect.right, m_DirtyRect.bottom);
                SelectClipRgn(hDC, aRgn);
                DeleteObject(aRgn);
            }

            RECTL aRect = {x, y, x + m_nWidth, y + m_nHeight};
            aViewObject->Draw(DVASPECT_CONTENT, 1,
                              NULL, NULL, NULL, hDC, &aRect, NULL, NULL,
                              0);

            //aViewObject->draw(DVASPECT_CONTENT, 1,
            //	NULL, NULL, NULL, hDC, NULL, NULL, NULL,
            //	0);

            aViewObject->Release();
        }
    }

    m_FlashDirty = false;
}

void Flash_Player::mouse_move(int x, int y) {
    LRESULT aResult;
    m_WindowlessObject->OnWindowMessage(WM_MOUSEMOVE, 0, MAKELPARAM(x, y), &aResult);

    RECT aRect	= {0, 0, m_nWidth, m_nHeight};
    POINT aPt	= {x, y};
    if(PtInRect(&aRect, aPt)) {
        do_check_cursor();
    }
}

void Flash_Player::mouse_down(int x, int y) {
    LRESULT aResult;
    m_WindowlessObject->OnWindowMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y), &aResult);
    do_check_cursor();
}

void Flash_Player::mouse_up(int x, int y) {
    LRESULT aResult;
    m_WindowlessObject->OnWindowMessage(WM_LBUTTONUP, 0, MAKELPARAM(x, y), &aResult);
    do_check_cursor();
}

void Flash_Player::mouse_leave() {
    // To prevent Flash control from setting our cursor if it thinks
    // our m_ouse is still over something
    mouse_move(-1, -1);
}

bool Flash_Player::is_dirty() {
    return m_FlashDirty;
}

void Flash_Player::set_dirty() {
    m_FlashDirty	= true;
    SetRect(&m_DirtyRect, 0, 0, m_nWidth, m_nHeight);
}

int Flash_Player::get_total_frames() {
    return m_nTotalFrames;
}

int Flash_Player::get_percent_loaded() {
    return m_nLoadingPercent;
}

HDC Flash_Player::get_HDC() {
    return m_hDC;
}

HBITMAP Flash_Player::get_bitmap() {
    return	m_hBmp;
}

COLORREF Flash_Player::get_bg_color() {
    return	m_bkColor;
}

void Flash_Player::set_bk_color(COLORREF color) {
    m_bkColor	= color;
}

int Flash_Player::get_width() {
    return m_nWidth;
}

int Flash_Player::get_height() {
    return m_nHeight;
}

void Flash_Player::do_on_flash_command( const char* szCommand, const char* szArgs ) {
    if(event_flash_command) {
        event_flash_command(this, szCommand, szArgs);
    }
}
