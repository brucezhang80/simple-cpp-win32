#ifndef EXEC_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define EXEC_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<string>

#include	"_third/ExecApp/ExecApp.h"

inline	bool	Exec_Cmd(
    LPCTSTR lpszCommandLine,		// complete command line, includeing exe
    WORD wShowCmd = SW_SHOWNORMAL
) {
    return	FALSE != ExecApp(lpszCommandLine, wShowCmd);
}

inline	bool	Exec_RegisteredApp(
    LPCTSTR lpszArgs,				// args for exe
    LPCTSTR lpszExt,				// file extension to use for exe lookup
    WORD wShowCmd = SW_SHOWNORMAL
) {
    return	FALSE != ExecRegisteredApp(lpszArgs, lpszExt, wShowCmd);
}

inline	bool	Exec_LookupRegisteredApp(
    LPCTSTR lpszExt,				// file extension to use for exe lookup
    LPTSTR lpszApplication,			// output path buffer
    DWORD *nSize					// size of buffer in TCHARs
) {
    return	FALSE != LookupRegisteredApp(lpszExt, lpszApplication, nSize);
}

DWORD	Exec_App(
    const char*		t_Comandline,		// IN
    DWORD			u32_FirstConvert,	// IN
    const char*		t_CurrentDir,		// IN
    const char*		t_UserVariables,	// IN
    BOOL			b_SeparatePipes,	// IN
    DWORD			u32_Timeout,		// IN
    std::string*	bs_ApiError,		// OUT
    std::string*	bs_StdOut,			// OUT
    std::string*	bs_StdErr			// OUT
);

#endif
