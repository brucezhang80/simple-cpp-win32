#include "stdafx.h"
#include "ComLibrary.h"

/*============================================================================
 * class ComLibrary
 */
//----------------------------------------------------------------------------
// CTOR
ComLibrary::ComLibrary() :
    mhDll(nullptr),
    mDllGetClassObject(nullptr),
    mDllCanUnloadNow(nullptr)
{
}

//----------------------------------------------------------------------------
// DTOR
ComLibrary::~ComLibrary()
{
  // force unloading
  Unload(TRUE);
}

//----------------------------------------------------------------------------
// Load: load a module from a certain path
HRESULT ComLibrary::Load(LPOLESTR szDllName)
{
  if (mhDll)
    { return S_FALSE; }  // is already loaded

  // Add a bit of securty:
  // 1) deny network paths
  if (PathIsNetworkPathW(szDllName))
      { return E_INVALIDARG; }

  // 2) deny relative paths
  if (PathIsNetworkPathW(szDllName))
      { return E_INVALIDARG; }
  
  // load library
  mhDll = ::CoLoadLibrary(szDllName, FALSE);
  if (!mhDll)
    { return HRESULT_FROM_WIN32(::GetLastError()); }

  // get DllGetClassObject and DllCanUnloadNow
  mDllGetClassObject = (FnDllGetClassObject)::GetProcAddress(mhDll, "DllGetClassObject");
  HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());

  // DllCanUnloadNow is optional
  mDllCanUnloadNow = (FnDllCanUnloadNow)::GetProcAddress(mhDll, "DllCanUnloadNow");

  if (!mDllGetClassObject)
    { return hr; }

  return S_OK;
}

//----------------------------------------------------------------------------
// Unload
HRESULT ComLibrary::Unload(BOOL bForce)
{
  if (!mhDll)
    { return S_OK; } // not loaded or already unloaded

  // call DllCanUnloadNow
  HRESULT hr = this->CanUnloadNow();

  // if can NOT unload AND NOT force...
  if ( (S_OK != hr) && !bForce )
    { return hr; } // ... don't close

  // close: free and clean up
  mDllGetClassObject = nullptr;
  mDllCanUnloadNow = nullptr;
  ::CoFreeLibrary(mhDll);
  mhDll = nullptr;
  return S_OK;
}

//----------------------------------------------------------------------------
// GetClassObject
HRESULT ComLibrary::GetClassObject(REFCLSID aClsid,
  REFIID aIid, LPVOID FAR* ppv)
{
  return (mDllGetClassObject)
      ? mDllGetClassObject(aClsid, aIid, ppv)
      : E_UNEXPECTED;
}

//----------------------------------------------------------------------------
// CanUnloadNow
HRESULT ComLibrary::CanUnloadNow()
{
  return (mDllCanUnloadNow)
      ? mDllCanUnloadNow()
      : S_OK; // default: yes, can unload
}
