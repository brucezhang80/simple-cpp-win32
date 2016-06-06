// ExecApp.cpp  Version 1.0 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     ExecApp implements functions to execute an application, look up the
//     application registered for a file extension, and execute a registered
//     application.
//
// History
//     Version 1.0 - 2008 June 8
//     - Initial public release
//
// Public APIs:
//           NAME                              DESCRIPTION
//     ---------------------  -------------------------------------------------
//     ExecApp()              Run an application; replacement for WinExec()
//     ExecRegisteredApp()    Look up application registered for extension and 
//                            run it
//     LookupRegisteredApp()  Look up application registered for extension
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////


//=============================================================================
// Do NOT include stdafx.h.
// Set this file to "not using precompiled headers".
//=============================================================================

#include <windows.h>
#include <tchar.h>
#include "ExecApp.h"

#pragma warning(disable : 4996)	// disable bogus deprecation warning

//=============================================================================
// If you have the latest Platform SDK you can use this include instead of
// the definitions following
//#include <shlwapi.h>

#ifdef _INC_SHLWAPI
#pragma message("ExecApp.cpp: shlwapi.h has been included")
#else
#pragma message("ExecApp.cpp: defining shlwapi objects")
#define ASSOCF_NOTRUNCATE		0x020
#define ASSOCF_IGNOREUNKNOWN	0x400
typedef DWORD ASSOCF;
typedef enum 
{
	ASSOCSTR_COMMAND      = 1,	// shell\verb\command string
	ASSOCSTR_EXECUTABLE,		// the executable part of command string
	// ...
} ASSOCSTR;
#define LWSTDAPI  EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
LWSTDAPI AssocQueryStringW(ASSOCF flags, ASSOCSTR str, LPCWSTR pszAssoc, 
						   LPCWSTR pszExtra, LPWSTR pszOut, DWORD *pcchOut);
#endif // _INC_SHLWAPI
//=============================================================================

//=============================================================================
// The following links to shlwapi.dll
#pragma message("ExecApp.cpp: automatic link to shlwapi.lib")
#pragma comment(lib, "shlwapi.lib")
//=============================================================================

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop

//=============================================================================
// if you want to see the TRACE output, uncomment this line:
//#include "XTrace.h"
//=============================================================================


///////////////////////////////////////////////////////////////////////////////
//
// ExecApp()
//
// Purpose:     Runs the specified application (replacement for WinExec)
//
// Parameters:  lpszCommandLine - [in] command line (including exe filepath) 
//                                that is passed to CreateProcess()
//              wShowCmd        - [in] Specifies how app window is to be shown.
//                                See ShowWindow() in MSDN for possible values.
//
// Returns:     BOOL            - TRUE = CreateProcess() succeeded
//
BOOL ExecApp(LPCTSTR lpszCommandLine, WORD wShowCmd /*= SW_SHOWNORMAL*/)
{
	BOOL rc = FALSE;

	if (lpszCommandLine && (lpszCommandLine[0] != _T('\0')))
	{
		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = wShowCmd;

		PROCESS_INFORMATION pi = { 0 };

		rc = ::CreateProcess(NULL, (LPTSTR)lpszCommandLine, NULL, NULL, FALSE, 
				NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
		TRACE(_T("CreateProcess returned %d for <%s>\n"), rc, lpszCommandLine);

		// close process and thread handles now (app will continue to run)
		if (pi.hProcess)
			::CloseHandle(pi.hProcess);
		if (pi.hThread)
			::CloseHandle(pi.hThread);
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// _AssocQueryString() - private function
//
// Purpose:     Calls AssocQueryStringW() to get associated application.
//
// Parameters:  See AssocQueryString() in MSDN.
//
// Returns:     HRESULT - S_OK = AssocQueryStringW() succeeded
//
// Notes:       This code is necessary because AssocQueryStringA() doesn't 
//              work (confirmed by MS Tech Support), so for ANSI version
//              we convert parameters to Unicode and call wide version.
//
static HRESULT _AssocQueryString(ASSOCF flags, 
								 ASSOCSTR str, 
								 LPCTSTR pszAssoc, 
								 LPCTSTR pszExtra,	// may be NULL
								 LPTSTR pszOut, 
								 DWORD *pcchOut)	// size of pszOut in TCHARs
{
	HRESULT hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND);

	if (!pszOut || !pcchOut || (*pcchOut == 0) || 
		!pszAssoc || (pszAssoc[0] == _T('\0')))
	{
		TRACE(_T("ERROR: _AssocQueryString: bad parameters\n"));
		return hr;
	}

	*pszOut = 0;

#ifdef _UNICODE

	hr = AssocQueryStringW(flags, str, pszAssoc, pszExtra, pszOut, pcchOut);

#else

	// get size of buffer for pszAssoc
	int wlen = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszAssoc, -1, NULL, 0);
	TRACE(_T("wlen=%d\n"), wlen);
	WCHAR *pszAssocW = new WCHAR [wlen+16];
	pszAssocW[0] = 0;

	// convert pszAssoc to unicode
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszAssoc, -1, pszAssocW, wlen+2);

	// get size of buffer for pszExtra
	WCHAR *pszExtraW = NULL;
	if (pszExtra  && (pszExtra[0] != _T('\0')))
	{
		wlen = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszExtra, -1, NULL, 0);
		TRACE(_T("wlen=%d\n"), wlen);
		pszExtraW = new WCHAR [wlen+16];
		pszExtraW[0] = 0;

		// convert pszExtra to unicode
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszExtra, -1, pszExtraW, wlen+2);
	}

	// create buffer for wide-character output
	WCHAR *pszOutW = new WCHAR [MAX_PATH*3];
	pszOutW[0] = 0;
	DWORD dwOut = MAX_PATH*3 - 4;

	// get exe filepath
	hr = AssocQueryStringW(flags, str, pszAssocW, pszExtraW, pszOutW, &dwOut);

	// convert exe filepath to ansi
	WideCharToMultiByte(CP_ACP, 0, pszOutW, -1, pszOut, *pcchOut, NULL, NULL);
	*pcchOut = _tcslen(pszOut);

	delete [] pszAssocW;
	delete [] pszExtraW;
	delete [] pszOutW;

#endif

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// LookupRegisteredApp()
//
// Purpose:     Retrieves the application registered for the specified file 
//              extension
//
// Parameters:  lpszExt         - [in] file extension (e.g., ".txt") used to 
//                                look up application file path. Preceding '.' 
//                                is necessary.
//              lpszApplication - [out] application path buffer
//              nSize           - [in/out] size of path buffer in TCHARs
//              
// Returns:     BOOL            - TRUE = found registered app
//
// Notes:       AssocQueryString() is broken in Vista. If no application is 
//              associated with the file extension, in Vista the function returns 
//              the "Unknown" application, rather than an error code (as in XP).
//              Adding ASSOCF_IGNOREUNKNOWN to the flags parameter will make the
//              function behave as in XP. ASSOCF_IGNOREUNKNOWN is defined in the 
//              latest Platform SDK.
//
BOOL LookupRegisteredApp(LPCTSTR lpszExt,
						 LPTSTR lpszApplication, 
						 DWORD *nSize)
{
	TRACE(_T("in LookupRegisteredApp:  lpszExt=<%s>\n"), lpszExt);

	BOOL rc = FALSE;

	// get app associated with file extension
	HRESULT hr = _AssocQueryString(ASSOCF_IGNOREUNKNOWN|ASSOCF_NOTRUNCATE,
								   ASSOCSTR_EXECUTABLE, 
								   lpszExt, 
								   NULL, 
								   lpszApplication, 
								   nSize);
	TRACE(_T("hr=0x%X\n"), hr);

	if (SUCCEEDED(hr) && (lpszApplication[0] != _T('\0')))
		rc = TRUE;

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// ExecRegisteredApp()
//
// Purpose:     Runs the application registered for the specified file extension
//
// Parameters:  lpszArgs - [in] command line arguments that are passed to app
//                         via CreateProcess();  if not already in quotes ("),
//                         they will be enclosed in quotes before CreateProcess()
//                         is called.  May be NULL.
//              lpszExt  - [in] file extension (e.g., ".txt") used to look up
//                         application file path.  Preceding '.' is necessary.
//              wShowCmd - [in] Specifies how the app window is to be shown.
//                         See ShowWindow() in MSDN for possible values.
//
// Returns:     BOOL     - TRUE = found registered app; CreateProcess() succeeded
//
BOOL ExecRegisteredApp(LPCTSTR lpszArgs,	// may be NULL
					   LPCTSTR lpszExt, 
					   WORD wShowCmd /*= SW_SHOWNORMAL*/)
{
	TRACE(_T("in ExecRegisteredApp:  lpszArgs=<%s>  lpszExt=<%s>\n"), lpszArgs, lpszExt);

	BOOL rc = FALSE;

	TCHAR szExe[MAX_PATH*3];
	szExe[0] = _T('\0');

	DWORD dwOut = sizeof(szExe)/sizeof(TCHAR)-4;

	rc = LookupRegisteredApp(lpszExt, szExe, &dwOut);

	if (rc)
	{
		size_t nBufSize = _tcslen(szExe);
		if (lpszArgs)
			nBufSize += _tcslen(lpszArgs);
		nBufSize += 100;

		TCHAR *buf = new TCHAR [nBufSize];

		// enclose exe filepath in quotes
		_tcscpy(buf, _T("\""));
		_tcscat(buf, szExe);
		_tcscat(buf, _T("\""));

		if (lpszArgs && (lpszArgs[0] != _T('\0')))
		{
			if (lpszArgs[0] != _T('"'))
			{
				// add args in quotes
				_tcscat(buf, _T(" \""));
				_tcscat(buf, lpszArgs);
				_tcscat(buf, _T("\""));
			}
			else
			{
				// add args (already in quotes)
				_tcscat(buf, _T(" "));
				_tcscat(buf, lpszArgs);
			}
		}

		rc = ExecApp(buf, wShowCmd);

		delete [] buf;
	}

	return rc;
}
