///////////////////////////////////////////////////////////
// ComLibrary
// Allows in-proc server loading without the server being
// registered.
// It basically wraps DllGetClassObject(),
// IClassFactory::CreateInstance() and DllCanUnloadNow().
//
// Usage:
// Put a member somwhere:
//   ComLibrary mMyLib;
//
// Before you want to use it call:
//   HRESULT hr = mMyLib.load(L"C:\\absolute\\path\\to\\MyLib.dll");
//
// Note that the path has to be absolute and can't be a network path to
// prevent security issues with loading DLLs from somewhere unexpected.
//
// And when you finished:
//   mMyLib.unload();
//
// If you use the library from within another library it's
// probably a good idea to modify your DllCanUnloadNow()
// and call there also mMyLib.CanUnloadNow().

class ComLibrary
{
public:
  ComLibrary();
  virtual ~ComLibrary();

  HRESULT Load(LPOLESTR szDllName);
  HRESULT Unload(BOOL bForce = FALSE);

  // these two wrap the functions exported from the COM module
  HRESULT GetClassObject(REFCLSID aClsid, REFIID aIid, LPVOID FAR* ppv);
  HRESULT CanUnloadNow();

  // wraps DllGetClassObject and IClassFactory::CreateInstance
  template<class Iface> HRESULT CreateInstance(
      REFCLSID aClsid,
      Iface** aRetPtrPtr,
      IUnknown* aUnknownOuter = NULL)
  {
    if (!mDllGetClassObject)
      { return E_UNEXPECTED; } // missing DllGetClassObject

    // get class factory
    IClassFactory * pClassFactory = nullptr;
    HRESULT hr = this->GetClassObject(aClsid, IID_IClassFactory, (LPVOID*)&pClassFactory);
    if (FAILED(hr))
      { return hr; }
    hr = pClassFactory->CreateInstance(aUnknownOuter, __uuidof(Iface), (void**)aRetPtrPtr);
    pClassFactory->Release();
    return hr;
  }

private:
  // prevent copying
  ComLibrary(const ComLibrary &);
  ComLibrary operator = (const ComLibrary &);

  // type for DllGetClassObject
  typedef HRESULT (__stdcall *FnDllGetClassObject)(IN REFCLSID rclsid,
    IN REFIID riid, OUT LPVOID FAR* ppv);

  // type for DllCanUnloadNow
  typedef HRESULT (__stdcall *FnDllCanUnloadNow)(void);

  // module handle
  HMODULE mhDll;

  // DllGetClassObject
  FnDllGetClassObject mDllGetClassObject;

  // DllCanUnloadNow
  FnDllCanUnloadNow   mDllCanUnloadNow;
};

