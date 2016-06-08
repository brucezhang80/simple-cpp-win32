// Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ComLibrary.h"

// for the CLSIDs
#include "../SampleModule/SampleModule_i.c"

int _tmain(int argc, _TCHAR* argv[])
{
  ComLibrary gSampleModule;

  // format path to module
  CAtlStringW sFilename, s;
  LPWSTR buffer = s.GetBuffer(_MAX_PATH);
  ::GetModuleFileNameW(nullptr, buffer, _MAX_PATH);
  ::PathRemoveFileSpecW(buffer);
  s.ReleaseBuffer();
  sFilename.Format(_T("%s\\%s"), s, _T("SampleModule.dll"));

  // load module
  HRESULT hr = gSampleModule.Load(sFilename.GetBuffer(sFilename.GetLength()));
  ATLASSERT(SUCCEEDED(hr));
  if (FAILED(hr)) {
    // add some proper error handling here
    return -1;
  }

  // Now we can start using it.
  // Create a new instance of CFoobar (which implements IFoobar and is
  // identified by CLSID_Foobar).
  CComPtr<IFoobar> foobar;
  hr = gSampleModule.CreateInstance(CLSID_Foobar, &foobar.p);
  ATLASSERT(SUCCEEDED(hr));

  // and use it
  if (SUCCEEDED(hr)) {
    INT result = foobar->DoSomethingUseful(3,4);
    _tprintf(_T("Result is: %i\n"), result);
    foobar.Release();
  }

  // We are done using SampleModule, unload it:
  gSampleModule.Unload();

  return 0;
}

