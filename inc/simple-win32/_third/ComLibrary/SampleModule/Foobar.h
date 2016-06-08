// Foobar.h : Declaration of the CFoobar

#pragma once
#include "resource.h"       // main symbols

#include "SampleModule_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

// CFoobar
class ATL_NO_VTABLE CFoobar :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFoobar, &CLSID_Foobar>,
	public IFoobar
{
public:
	CFoobar()
	{
	}

  DECLARE_NO_REGISTRY()
  DECLARE_NOT_AGGREGATABLE(CFoobar)
  BEGIN_COM_MAP(CFoobar)
	  COM_INTERFACE_ENTRY(IFoobar)
  END_COM_MAP()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
  STDMETHOD_(INT, DoSomethingUseful)(INT a, INT b);
};

OBJECT_ENTRY_AUTO(__uuidof(Foobar), CFoobar)
