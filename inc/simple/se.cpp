#include	<new>
#include	<Windows.h>
#include	"se.h"
#include	"simple/string.h"

//
//	获取异常
//
#if _MSC_VER>=1300
#include <rtcapi.h>
#endif

#ifndef _AddressOfReturnAddress
// Taken from: http://msdn.microsoft.com/en-us/library/s975zw7k(VS.71).aspx
// _ReturnAddress and _AddressOfReturnAddress should be prototyped before use
extern "C"	void * _AddressOfReturnAddress(void);
extern "C"	void * _ReturnAddress(void);
#endif

// The following function retrieves exception info
EXCEPTION_POINTERS* se_get_exception_pointers(DWORD dwExceptionCode) {
    // The following code was taken from VC++ 8.0 CRT (invarg.c: line 104)
    EXCEPTION_RECORD ExceptionRecord;
    CONTEXT ContextRecord;
    memset(&ContextRecord, 0, sizeof(CONTEXT));

#ifdef _X86_
    __asm {
        mov dword ptr [ContextRecord.Eax], eax
        mov dword ptr [ContextRecord.Ecx], ecx
        mov dword ptr [ContextRecord.Edx], edx
        mov dword ptr [ContextRecord.Ebx], ebx
        mov dword ptr [ContextRecord.Esi], esi
        mov dword ptr [ContextRecord.Edi], edi
        mov word ptr [ContextRecord.SegSs], ss
        mov word ptr [ContextRecord.SegCs], cs
        mov word ptr [ContextRecord.SegDs], ds
        mov word ptr [ContextRecord.SegEs], es
        mov word ptr [ContextRecord.SegFs], fs
        mov word ptr [ContextRecord.SegGs], gs
        pushfd
        pop [ContextRecord.EFlags]
    }
    ContextRecord.ContextFlags = CONTEXT_CONTROL;
#	pragma warning(push)
#	pragma warning(disable:4311)
    ContextRecord.Eip = (ULONG)_ReturnAddress();
    ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#	pragma warning(pop)
    ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);
#elif defined (_IA64_) || defined (_AMD64_)
    /* Need to fill up the Context in IA64 and AMD64. */
    RtlCaptureContext(&ContextRecord);
#else  /* defined (_IA64_) || defined (_AMD64_) */
    ZeroMemory(&ContextRecord, sizeof(ContextRecord));
#endif  /* defined (_IA64_) || defined (_AMD64_) */

    ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));
    ExceptionRecord.ExceptionCode = dwExceptionCode;
    ExceptionRecord.ExceptionAddress = _ReturnAddress();

    EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
    memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
    CONTEXT* pContextRecord = new CONTEXT;
    memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

    EXCEPTION_POINTERS*	pExceptionPointers	= new EXCEPTION_POINTERS;
    pExceptionPointers->ExceptionRecord = pExceptionRecord;
    pExceptionPointers->ContextRecord = pContextRecord;

    return	pExceptionPointers;
}

void se_destroy_exception_pointers(EXCEPTION_POINTERS* ppExceptionPointers) {
    if(NULL == ppExceptionPointers) {
        return;
    }

    delete	ppExceptionPointers->ExceptionRecord;
    delete	ppExceptionPointers->ContextRecord;
    delete	ppExceptionPointers;
}

//-----------------------------------------------------------------------------
// Name: get_impersonation_token()
// Desc: The method acts as a potential workaround for the fact that the
//       current thread may not have a token assigned to it, and if not, the
//       process token is received.
static	bool	get_impersonation_token( HANDLE* phToken ) {
    if(NULL == phToken) {
        return	false;
    }

    *phToken = NULL;
    if( OpenThreadToken( GetCurrentThread(),
                         TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
                         TRUE,
                         phToken) ) {
        return true;
    }

    if( ERROR_NO_TOKEN != GetLastError() ) {
        return false;
    }

    // No impersonation token for the current thread is available.
    // Let's go for the process token instead.
    if( OpenProcessToken( GetCurrentProcess(),
                          TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
                          phToken) ) {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name: enable_debug_privilege()
// Desc: Since a MiniDump contains a lot of meta-data about the OS and
//       application state at the time of the dump, it is a rather privileged
//       operation. This means we need to set the SeDebugPrivilege to be able
//       to call MiniDumpWriteDump.
//-----------------------------------------------------------------------------
static	bool	enable_debug_privilege( LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld ) {
    BOOL bOk = FALSE;

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    bOk = LookupPrivilegeValue( 0, pszPriv, &tp.Privileges[0].Luid );

    if( bOk ) {
        DWORD cbOld = sizeof(*ptpOld);
        bOk = AdjustTokenPrivileges( hToken, FALSE, &tp, cbOld, ptpOld, &cbOld );
    }

    return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}

//-----------------------------------------------------------------------------
// Name: restore_token_privilege()
// Desc:
//-----------------------------------------------------------------------------
bool	restore_token_privilege( HANDLE hToken, TOKEN_PRIVILEGES* ptpOld ) {
    BOOL bOk = AdjustTokenPrivileges(hToken, FALSE, ptpOld, 0, NULL, NULL);
    return ( bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()) );
}

//
//	结构化异常基类
//
se_exception::se_exception(unsigned int code, _EXCEPTION_POINTERS* pep): code_(code), pep_(pep) {
    if(NULL == pep) {
        string_format(msg_, "Exception 0x%08x", code);
    } else {
        char szModuleName[MAX_PATH] = {0};
        se_module(szModuleName, sizeof(szModuleName));
        string_format(msg_, "Exception Module:%s Type:%ld Flags:%ld Address:%0X",
                      szModuleName,
                      pep->ExceptionRecord->ExceptionCode,
                      pep->ExceptionRecord->ExceptionFlags,
                      (UINT_PTR)pep->ExceptionRecord->ExceptionAddress
                     );
    }
}

void	se_exception::se_module(char* pszModuleName, size_t size)const {
    HMODULE hModule = NULL;
#if _WIN32_WINNT > 0x0500 || defined(WINBASE_DECLARE_GET_MODULE_HANDLE_EX) || ISOLATION_AWARE_ENABLED
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)se_pointers()->ExceptionRecord->ExceptionAddress, &hModule);
#endif
    GetModuleFileName(hModule, pszModuleName, DWORD(size));
    if(NULL != hModule) {
        CloseHandle(hModule);
    }
}

void	se_exception::generate_dump()const {
    char	sFile[MAX_PATH]	= {0};
    if(!on_get_dump_file_name(sFile, sizeof(sFile))) {
        return;
    }

    HANDLE hFile = CreateFile( sFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if(NULL == hFile) {
        return;
    }

    {
        HANDLE hImpersonationToken = NULL;
        get_impersonation_token(&hImpersonationToken);

        TOKEN_PRIVILEGES	tp;
        bool bPrivilegeEnabled =	false;
        if(NULL != hImpersonationToken) {
            bPrivilegeEnabled	= enable_debug_privilege(SE_DEBUG_NAME, hImpersonationToken, &tp);
        }

        on_generate_dump( hFile );

        if( bPrivilegeEnabled ) {
            restore_token_privilege(hImpersonationToken, &tp);
        }
    }

    CloseHandle( hFile );
}

bool	se_exception::on_get_dump_file_name(char* pszFileName, size_t size)const {
    char szModuleName[MAX_PATH] = {0};
    se_module(szModuleName, sizeof(szModuleName));

    SYSTEMTIME stLocalTime;
    GetLocalTime( &stLocalTime );

    _snprintf( pszFileName, size, "%s_%04d-%02d-%02d_%02d-%02d-%02d.dmp\0",
               szModuleName, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

    return	true;
}
DWORD	se_exception::on_get_dump_data_falgs()const {
    return	MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithProcessThreadData | MiniDumpWithDataSegs;
}
bool	se_exception::on_generate_dump(HANDLE hFile)const {
    //
    //	Note: MiniDumpWriteDump is not THREAD-SAFE !!!
    //
    //	如果要在多线程环境中调用该函数，可参考以下步骤：
    //	{
    //		EnterCriticalSection( s_pCriticalSection );
    //		__super::on_generate_dump_file(hFile);
    //		LeaveCriticalSection( s_pCriticalSection );
    //	}
    //

    MINIDUMP_EXCEPTION_INFORMATION eInfo;
    eInfo.ThreadId = GetCurrentThreadId(); //把需要的信息添进去
    eInfo.ExceptionPointers = se_pointers();
    eInfo.ClientPointers = FALSE;

    return (0 != MiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hFile,
                static_cast<MINIDUMP_TYPE>(on_get_dump_data_falgs()),
                (NULL != eInfo.ExceptionPointers) ? &eInfo : NULL,
                NULL,
                NULL));
}
