// ExecApp.h  Version 1.0 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
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

#ifndef EXECAPP_H
#define EXECAPP_H

BOOL ExecApp(LPCTSTR lpszCommandLine,		// complete command line, includeing exe
			 WORD wShowCmd = SW_SHOWNORMAL);

BOOL ExecRegisteredApp(LPCTSTR lpszArgs,	// args for exe 
					   LPCTSTR lpszExt,		// file extension to use for exe lookup
					   WORD wShowCmd = SW_SHOWNORMAL);

BOOL LookupRegisteredApp(LPCTSTR lpszExt,	// file extension to use for exe lookup
						 LPTSTR lpszApplication,	// output path buffer
						 DWORD *nSize);		// size of buffer in TCHARs

#endif //EXECAPP_H
