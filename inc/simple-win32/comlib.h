///////////////////////////////////////////////////////////
// ComLib
// Allows in-proc server loading without the server being
// registered.
// It basically wraps DllGetClassObject(),
// IClassFactory::create_instance() and DllCanUnloadNow().
//
// Usage:
// Put a member somwhere:
//   ComLib mMyLib;
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
// and call there also mMyLib.can_unload_now().

class ComLib {
public:
    ComLib();
    virtual ~ComLib();

    HRESULT load(LPOLESTR szDllName);
    HRESULT unload(BOOL bForce = FALSE);

    // these two wrap the functions exported from the COM module
    HRESULT get_class_object(REFCLSID aClsid, REFIID aIid, LPVOID FAR* ppv);
    HRESULT can_unload_now();

    // wraps DllGetClassObject and IClassFactory::create_instance
    template<class Iface> HRESULT create_instance(
        REFCLSID aClsid,
        Iface** aRetPtrPtr,
        IUnknown* aUnknownOuter = NULL) {
        if (!mDllGetClassObject) {
            return E_UNEXPECTED;    // missing DllGetClassObject
        }

        // get class factory
        IClassFactory * pClassFactory = NULL;
        HRESULT hr = this->get_class_object(aClsid, IID_IClassFactory, (LPVOID*)&pClassFactory);
        if (FAILED(hr)) {
            return hr;
        }
        hr = pClassFactory->create_instance(aUnknownOuter, __uuidof(Iface), (void**)aRetPtrPtr);
        pClassFactory->Release();
        return hr;
    }

private:
    // prevent copying
    ComLib(const ComLib &);
    ComLib operator = (const ComLib &);

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

