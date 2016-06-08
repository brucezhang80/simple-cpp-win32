

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Fri Oct 09 19:30:12 2015
 */
/* Compiler settings for SampleModule.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __SampleModule_i_h__
#define __SampleModule_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFoobar_FWD_DEFINED__
#define __IFoobar_FWD_DEFINED__
typedef interface IFoobar IFoobar;

#endif 	/* __IFoobar_FWD_DEFINED__ */


#ifndef __Foobar_FWD_DEFINED__
#define __Foobar_FWD_DEFINED__

#ifdef __cplusplus
typedef class Foobar Foobar;
#else
typedef struct Foobar Foobar;
#endif /* __cplusplus */

#endif 	/* __Foobar_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IFoobar_INTERFACE_DEFINED__
#define __IFoobar_INTERFACE_DEFINED__

/* interface IFoobar */
/* [unique][local][uuid][object] */ 


EXTERN_C const IID IID_IFoobar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F79AC74-12EE-44B6-8000-D6EBECB8F5E4")
    IFoobar : public IUnknown
    {
    public:
        virtual INT STDMETHODCALLTYPE DoSomethingUseful( 
            INT a,
            INT b) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFoobarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFoobar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFoobar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFoobar * This);
        
        INT ( STDMETHODCALLTYPE *DoSomethingUseful )( 
            IFoobar * This,
            INT a,
            INT b);
        
        END_INTERFACE
    } IFoobarVtbl;

    interface IFoobar
    {
        CONST_VTBL struct IFoobarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFoobar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFoobar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFoobar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFoobar_DoSomethingUseful(This,a,b)	\
    ( (This)->lpVtbl -> DoSomethingUseful(This,a,b) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFoobar_INTERFACE_DEFINED__ */



#ifndef __SampleModuleLib_LIBRARY_DEFINED__
#define __SampleModuleLib_LIBRARY_DEFINED__

/* library SampleModuleLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_SampleModuleLib;

EXTERN_C const CLSID CLSID_Foobar;

#ifdef __cplusplus

class DECLSPEC_UUID("530BD770-9EEE-4357-8A69-75A095834D2B")
Foobar;
#endif
#endif /* __SampleModuleLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


