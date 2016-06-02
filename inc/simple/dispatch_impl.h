#ifndef DISPATCHIMPL_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define DISPATCHIMPL_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include "oaidl.h"
#include "comutil.h"

/*
* DispatchImpl : a simple dispatch driver used for script
*/
class DispatchImpl : public IDispatch {
public:
    DispatchImpl(bool bAutoDelete = false, bool bZeroRefCount = false): m_uRefCount(1), m_bAutoDelete(bAutoDelete) {
        if (bZeroRefCount) {
            m_uRefCount = 0;
        }
    }

    virtual ~DispatchImpl(void) {
        // NULL;
    }
public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void ** ppvObject) {
        if ( riid == IID_IUnknown || riid == IID_IDispatch ) {
            this->AddRef();
            *ppvObject = this;
            return S_OK;
        } else {
            *ppvObject = NULL;
            return E_NOINTERFACE;
        }
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void) {
        return ++m_uRefCount;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void) {
        ULONG ret = --m_uRefCount;
        if ( ret == 0 && m_bAutoDelete ) {
            delete this;
        }
        return ret;
    }

    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
        /* [out] */ UINT *pctinfo) {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ ITypeInfo **ppTInfo) {
        return E_NOTIMPL;
    }

    virtual HRESULT GetIDOrInvoke(BOOL bGetID, WORD wFlags, LPOLESTR szName, DISPPARAMS *pDispParams, VARIANT *pVarResult, DISPID* rgDispID) = 0 {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
        /* [in] */ REFIID riid,
        /* [size_is][in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ DISPID *rgDispId) {
        return GetIDOrInvoke(TRUE, 0, rgszNames[0], NULL, NULL, rgDispId);
    }

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
        /* [in] */ DISPID dispIdMember,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS *pDispParams,
        /* [out] */ VARIANT *pVarResult,
        /* [out] */ EXCEPINFO *pExcepInfo,
        /* [out] */ UINT *puArgErr) {
        return GetIDOrInvoke(FALSE, wFlags, NULL, pDispParams, pVarResult, &dispIdMember);
    }

private:
    int		m_uRefCount;
    bool	m_bAutoDelete;
};

//
//	DISPATCH_ITEMS_BEGIN()
//
//	DISPATCH_ITEMS_END()
//

#define VARIANT_INIT_AND_COPY(varDest, varSrc)	\
	{\
		VariantInit(&varDest);\
		VariantCopy(&varDest, &varSrc);\
	}

#define DISPATCH_ITEMS_BEGIN(theClass) \
	typedef	theClass	DispatchImpl__THIS_CLASS;\
	typedef void (theClass::*_AC_DispP0Func)(_variant_t&);\
	HRESULT MultiParamInvoke(_AC_DispP0Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 0 ) return E_INVALIDARG;\
 _variant_t retval;\
 (this->*func)(retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
	typedef void (theClass::*_AC_DispP1Func)(_variant_t, _variant_t&);\
	HRESULT MultiParamInvoke(_AC_DispP1Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 1 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[1 - 1 - 0]);\
 _variant_t retval;\
 (this->*func)(v1, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP2Func)(_variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP2Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 2 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[2 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[2 - 1 - 1]);\
 _variant_t retval;\
 (this->*func)(v1, v2, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);;\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP3Func)(_variant_t, _variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP3Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 3 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[3 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[3 - 1 - 1]);\
 _variant_t v3(pDispParams->rgvarg[3 - 1 - 2]);\
 _variant_t retval;\
 (this->*func)(v1, v2, v3, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP4Func)(_variant_t, _variant_t, _variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP4Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 4 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[4 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[4 - 1 - 1]);\
 _variant_t v3(pDispParams->rgvarg[4 - 1 - 2]);\
 _variant_t v4(pDispParams->rgvarg[4 - 1 - 3]);\
 _variant_t retval;\
 (this->*func)(v1, v2, v3, v4, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP5Func)(_variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP5Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 5 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[5 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[5 - 1 - 1]);\
 _variant_t v3(pDispParams->rgvarg[5 - 1 - 2]);\
 _variant_t v4(pDispParams->rgvarg[5 - 1 - 3]);\
 _variant_t v5(pDispParams->rgvarg[5 - 1 - 4]);\
 _variant_t retval;\
 (this->*func)(v1, v2, v3, v4, v5, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP6Func)(_variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP6Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 6 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[6 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[6 - 1 - 1]);\
 _variant_t v3(pDispParams->rgvarg[6 - 1 - 2]);\
 _variant_t v4(pDispParams->rgvarg[6 - 1 - 3]);\
 _variant_t v5(pDispParams->rgvarg[6 - 1 - 4]);\
 _variant_t v6(pDispParams->rgvarg[6 - 1 - 5]);\
 _variant_t retval;\
 (this->*func)(v1, v2, v3, v4, v5, v6, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP7Func)(_variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP7Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 7 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[7 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[7 - 1 - 1]);\
 _variant_t v3(pDispParams->rgvarg[7 - 1 - 2]);\
 _variant_t v4(pDispParams->rgvarg[7 - 1 - 3]);\
 _variant_t v5(pDispParams->rgvarg[7 - 1 - 4]);\
 _variant_t v6(pDispParams->rgvarg[7 - 1 - 5]);\
 _variant_t v7(pDispParams->rgvarg[7 - 1 - 6]);\
 _variant_t retval;\
 (this->*func)(v1, v2, v3, v4, v5, v6, v7, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP8Func)(_variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP8Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 8 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[8 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[8 - 1 - 1]);\
 _variant_t v3(pDispParams->rgvarg[8 - 1 - 2]);\
 _variant_t v4(pDispParams->rgvarg[8 - 1 - 3]);\
 _variant_t v5(pDispParams->rgvarg[8 - 1 - 4]);\
 _variant_t v6(pDispParams->rgvarg[8 - 1 - 5]);\
 _variant_t v7(pDispParams->rgvarg[8 - 1 - 6]);\
 _variant_t v8(pDispParams->rgvarg[8 - 1 - 7]);\
 _variant_t retval;\
 (this->*func)(v1, v2, v3, v4, v5, v6, v7, v8, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP9Func)(_variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP9Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 9 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[9 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[9 - 1 - 1]);\
 _variant_t v3(pDispParams->rgvarg[9 - 1 - 2]);\
 _variant_t v4(pDispParams->rgvarg[9 - 1 - 3]);\
 _variant_t v5(pDispParams->rgvarg[9 - 1 - 4]);\
 _variant_t v6(pDispParams->rgvarg[9 - 1 - 5]);\
 _variant_t v7(pDispParams->rgvarg[9 - 1 - 6]);\
 _variant_t v8(pDispParams->rgvarg[9 - 1 - 7]);\
 _variant_t v9(pDispParams->rgvarg[9 - 1 - 8]);\
 _variant_t retval;\
 (this->*func)(v1, v2, v3, v4, v5, v6, v7, v8, v9, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 typedef void (theClass::*_AC_DispP10Func)(_variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t, _variant_t&);\
 HRESULT MultiParamInvoke(_AC_DispP10Func func, DISPPARAMS *pDispParams, VARIANT *pVarResult)\
 {\
 if ( pDispParams->cArgs != 10 ) return E_INVALIDARG;\
 _variant_t v1(pDispParams->rgvarg[10 - 1 - 0]);\
 _variant_t v2(pDispParams->rgvarg[10 - 1 - 1]);\
 _variant_t v3(pDispParams->rgvarg[10 - 1 - 2]);\
 _variant_t v4(pDispParams->rgvarg[10 - 1 - 3]);\
 _variant_t v5(pDispParams->rgvarg[10 - 1 - 4]);\
 _variant_t v6(pDispParams->rgvarg[10 - 1 - 5]);\
 _variant_t v7(pDispParams->rgvarg[10 - 1 - 6]);\
 _variant_t v8(pDispParams->rgvarg[10 - 1 - 7]);\
 _variant_t v9(pDispParams->rgvarg[10 - 1 - 8]);\
 _variant_t v10(pDispParams->rgvarg[10 - 1 - 9]);\
 _variant_t retval;\
 (this->*func)(v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, retval);\
 if (pVarResult) VARIANT_INIT_AND_COPY(*pVarResult , retval);\
 return S_OK;\
 }\
 virtual HRESULT GetIDOrInvoke(BOOL bGetID, WORD wFlags, LPOLESTR szName, DISPPARAMS *pDispParams, VARIANT *pVarResult, DISPID* rgDispID)\
 {\


#define DISPATCH_ITEMS_END() \
	return __super::GetIDOrInvoke(bGetID, wFlags, szName, pDispParams, pVarResult, rgDispID);\
 }


#define DISPATCH_FUNCTION(id, funcName, funcptr) \
	if ( bGetID && wcscmp(szName, funcName) == 0 )\
  {\
  *rgDispID = id;\
  return S_OK; \
  }\
  else if ( (!bGetID) && *rgDispID == id ) \
  { \
  return MultiParamInvoke(&DispatchImpl__THIS_CLASS:: funcptr, pDispParams, pVarResult);\
  }

#define DISPATCH_PROP(id, propName, getfunc, putfunc) \
	if ( bGetID && wcscmp(szName, propName) == 0 )\
 {\
 *rgDispID = id;\
 return S_OK;\
 }\
 else if ( (!bGetID) && *rgDispID == id ) \
 { \
 if ( wFlags & (INVOKE_PROPERTYPUT | INVOKE_PROPERTYPUTREF) ) \
  { \
  if ( pDispParams->cNamedArgs != 1 || pDispParams->rgdispidNamedArgs[0] != DISPID_PROPERTYPUT ) return DISP_E_MEMBERNOTFOUND; \
  putfunc(_variant_t(pDispParams->rgvarg[0]));\
  return S_OK; \
  } \
  else \
  { \
  if ( pDispParams->cArgs != 0 ) return DISP_E_BADPARAMCOUNT; \
  _variant_t retval; \
  getfunc(retval); \
  *pVarResult = retval; \
  return S_OK; \
  } \
 }

#define DISPATCH_READONLYPROP(id, propName, getfunc) \
	if ( bGetID && wcscmp(szName, propName) == 0 )\
 {\
 *rgDispID = id;\
 return S_OK;\
 }\
 else if ( (!bGetID) && *rgDispID == id ) \
 { \
 if ( pDispParams->cArgs != 0 ) return DISP_E_BADPARAMCOUNT; \
 _variant_t retval; \
 getfunc(retval); \
 *pVarResult = retval; \
 return S_OK; \
 } \

#endif
