#include "stdafx.h"
#include <cassert>
#include <sstream>
#include "FlashCtrlEx.h"

#include "simple-win32/dynamic.h"

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

/////////////////////////////////////////////////////////////////////////////
// FlashCtrlEx

FlashCtrlEx::FlashCtrlEx()
    :	flash_call_handler_(NULL) {
}

FlashCtrlEx::~FlashCtrlEx() {
}

IMPLEMENT_DYNCREATE(FlashCtrlEx, FlashCtrl)

/////////////////////////////////////////////////////////////////////////////
// FlashCtrlEx 属性

/////////////////////////////////////////////////////////////////////////////
// FlashCtrlEx 操作
BEGIN_EVENTSINK_MAP(FlashCtrlEx, FlashCtrl)
ON_EVENT_REFLECT(FlashCtrlEx, DISPID_READYSTATECHANGE, Internal_OnReadyStateChange, VTS_I4)
ON_EVENT_REFLECT(FlashCtrlEx, 1958, Internal_OnProgress, VTS_I4)
ON_EVENT_REFLECT(FlashCtrlEx, 150, Internal_OnFSCommand, VTS_BSTR VTS_BSTR)
ON_EVENT_REFLECT(FlashCtrlEx, 197, Internal_OnFlashCall, VTS_BSTR)
END_EVENTSINK_MAP()

void FlashCtrlEx::Internal_OnReadyStateChange(long newState) {
    if(evtReadyStateChange && evtReadyStateChange(this, newState)) {
        return;
    }
}

void FlashCtrlEx::Internal_OnProgress(long percentDone) {
    if(evtLoadProgress && evtLoadProgress(this, percentDone)) {
        return;
    }
}

void FlashCtrlEx::Internal_OnFSCommand(LPCTSTR command, LPCTSTR args) {
    if(evtFSCommand && evtFSCommand(this, command, args)) {
        return;
    }
}

void FlashCtrlEx::Internal_OnFlashCall(LPCTSTR request) {
    if(evtFlashCall && evtFlashCall(this, request)) {
        return;
    }

    if(NULL == flash_call_handler_) {
        return;
    }

    std::string					func;
    std::deque<DynamicData*>	args;
    FlashCallXML_DeSerializeInvoke(request, func, args);
    if(!flash_call_handler_->call_function(func.c_str(), args)) {
        FlashCallXML_ClearArguments(args);
        return;
    }

    FlashCallXML_ClearArguments(args);
    DynamicData&	ret	= flash_call_handler_->get_return_value();
    if(DynamicData::VALUE_EMPTY != ret.type()) {
        std::stringstream	ss;
        FlashCallXML_SerializeData(ss, ret);
        SetReturnValue(ss.str().c_str());
    }
}

bool FlashCtrlEx::LoadMovie(void* data, size_t size) {
    FlashMemoryStream fStream(data,	size);
    LPUNKNOWN	pIntf	= this->GetControlUnknown();
    if(NULL == pIntf) {
        return	false;
    }

    IPersistStreamInit*	psStreamInit = NULL;
    HRESULT	hr	= pIntf->QueryInterface(IID_IPersistStreamInit,(LPVOID*)&psStreamInit);
    if(SUCCEEDED(hr)) {
        psStreamInit->InitNew();
        psStreamInit->Load((LPSTREAM)&fStream);
        psStreamInit->Release();
        return	true;
    }

    return	false;
}

void FlashCtrlEx::LoadMovie(long layer, const char* url) {
    FlashCtrl::LoadMovie(layer, url);
}

bool FlashCtrlEx::Internal_CallFunction(const char* pszFunction, std::deque<DynamicData*>& args) {
    ret_.Empty();

    std::stringstream	ss;
    if(!FlashCallXML_SerializeInvoke(ss, pszFunction, args)) {
        return	false;
    }

    try {
        ret_	= CallFunction(ss.str().c_str());
    } catch(COleDispatchException* pEx) {
        assert(NULL == "调用Flash函数异常，函数名错误？");
        pEx->Delete();
        return	false;
    }

    return	true;
}

