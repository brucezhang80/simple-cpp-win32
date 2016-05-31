#ifndef EXCEPTION_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define EXCEPTION_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<new>
#include	<string>
#include	<exception>
#include	<eh.h>
#include	<dbghelp.h>
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
inline	EXCEPTION_POINTERS* SEException_GetExceptionPointers(DWORD dwExceptionCode)
{
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

inline	void SEException_DestroyExceptionPointers(EXCEPTION_POINTERS* ppExceptionPointers)
{
	if(NULL == ppExceptionPointers)
	{
		return;
	}

	delete	ppExceptionPointers->ExceptionRecord;
	delete	ppExceptionPointers->ContextRecord;
	delete	ppExceptionPointers;
}

//
//	结构化异常基类
//
struct SEExceptionBase : public std::exception
{
public:
	SEExceptionBase(unsigned int code, _EXCEPTION_POINTERS* pep): code_(code), pep_(pep)
	{
		if(NULL == pep)
		{
			StringFormat<256>(msg_, "Exception 0x%08x", code);
		}
		else
		{
			char szModuleName[MAX_PATH] = {0};
			GetExceptionModuleFileName(szModuleName, sizeof(szModuleName));
			StringFormat<1024>(msg_, "Exception Module:%s Type:%ld Flags:%ld Address:%0X", 
				szModuleName,
				pep->ExceptionRecord->ExceptionCode,
				pep->ExceptionRecord->ExceptionFlags, 
				(UINT_PTR)pep->ExceptionRecord->ExceptionAddress
				);
		}
	}

	unsigned int				GetExceptionCode()const												{ return code_; }
	_EXCEPTION_POINTERS*		GetExceptionPointers()const											{ return pep_;	}
	void						GetExceptionModuleFileName(char* pszModuleName, size_t size)const
	{
		HMODULE hModule = NULL;
#if _WIN32_WINNT > 0x0500 || defined(WINBASE_DECLARE_GET_MODULE_HANDLE_EX) || ISOLATION_AWARE_ENABLED
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetExceptionPointers()->ExceptionRecord->ExceptionAddress, &hModule);
#endif
		GetModuleFileName(hModule, pszModuleName, DWORD(size));
		if(NULL != hModule)
		{
			CloseHandle(hModule);
		}
	}

public:
	void						GenerateDumpFile()const
	{
		char	sFile[MAX_PATH]	= {0};
		if(!OnGetDumpFileName(sFile, sizeof(sFile)))
		{
			return;
		}

		HANDLE hFile = CreateFile( sFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 
		if(NULL == hFile)
		{
			return;
		}

		{
			HANDLE hImpersonationToken = NULL;
			GetImpersonationToken(&hImpersonationToken);

			TOKEN_PRIVILEGES	tp;
			bool bPrivilegeEnabled =	false;
			if(NULL != hImpersonationToken)
			{
				bPrivilegeEnabled	= EnableDebugPrivilege(SE_DEBUG_NAME, hImpersonationToken, &tp);
			}

			OnGenerateDumpFile( hFile );

			if( bPrivilegeEnabled )
			{
				RestoreTokenPrivilege(hImpersonationToken, &tp);
			}
		}

		CloseHandle( hFile );
	}

public:
	static	void				OnThrowException(const SEExceptionBase& ex)							{}
	static	void				OnUnhandledException(const SEExceptionBase& ex)						{}
	static	void				OnNewException()													{ throw std::bad_alloc("no more memory!"); }

public:
	virtual const char *		what()const															{ return msg_.c_str();	}

protected:
	virtual	bool				OnGetDumpFileName(char* pszFileName, size_t size)const
	{
		char szModuleName[MAX_PATH] = {0};
		GetExceptionModuleFileName(szModuleName, sizeof(szModuleName));

		SYSTEMTIME stLocalTime;
		GetLocalTime( &stLocalTime );

		_sntprintf( pszFileName, size, "%s_%04d-%02d-%02d_%02d-%02d-%02d.dmp\0",
			szModuleName, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

		return	true;
	}
	virtual	DWORD				OnGetDumpDataFlags()const
	{
		return	MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithProcessThreadData | MiniDumpWithDataSegs;
	}
	virtual	bool				OnGenerateDumpFile(HANDLE hFile)const
	{
		//
		//	Note: MiniDumpWriteDump is not THREAD-SAFE !!!
		//
		//	如果要在多线程环境中调用该函数，可参考以下步骤：
		//	{
		//		EnterCriticalSection( s_pCriticalSection );
		//		__super::OnGenerateDumpFile(hFile);
		//		LeaveCriticalSection( s_pCriticalSection );
		//	}
		//

		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId(); //把需要的信息添进去
		eInfo.ExceptionPointers = GetExceptionPointers();
		eInfo.ClientPointers = FALSE;

		return (0 != MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			static_cast<MINIDUMP_TYPE>(OnGetDumpDataFlags()),
			(NULL != eInfo.ExceptionPointers) ? &eInfo : NULL,
			NULL,
			NULL));
	}

protected:
	//-----------------------------------------------------------------------------
	// Name: GetImpersonationToken()
	// Desc: The method acts as a potential workaround for the fact that the 
	//       current thread may not have a token assigned to it, and if not, the 
	//       process token is received.
	static	bool				GetImpersonationToken( HANDLE* phToken )
	{
		if(NULL == phToken)
		{
			return	false;
		}

		*phToken = NULL;
		if( OpenThreadToken( GetCurrentThread(),
			TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
			TRUE,
			phToken) )
		{
			return true;
		}

		if( ERROR_NO_TOKEN != GetLastError() )
		{
			return false;
		}

		// No impersonation token for the current thread is available. 
		// Let's go for the process token instead.
		if( OpenProcessToken( GetCurrentProcess(),
			TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
			phToken) )
		{
			return true;
		}
		
		return false;
	}

	//-----------------------------------------------------------------------------
	// Name: EnableDebugPrivilege()
	// Desc: Since a MiniDump contains a lot of meta-data about the OS and 
	//       application state at the time of the dump, it is a rather privileged 
	//       operation. This means we need to set the SeDebugPrivilege to be able 
	//       to call MiniDumpWriteDump.
	//-----------------------------------------------------------------------------
	static	bool				EnableDebugPrivilege( LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld )
	{
		BOOL bOk = FALSE;

		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		bOk = LookupPrivilegeValue( 0, pszPriv, &tp.Privileges[0].Luid );

		if( bOk )
		{
			DWORD cbOld = sizeof(*ptpOld);
			bOk = AdjustTokenPrivileges( hToken, FALSE, &tp, cbOld, ptpOld, &cbOld );
		}

		return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
	}

	//-----------------------------------------------------------------------------
	// Name: RestoreTokenPrivilege()
	// Desc: 
	//-----------------------------------------------------------------------------
	static	bool				RestoreTokenPrivilege( HANDLE hToken, TOKEN_PRIVILEGES* ptpOld )
	{
		BOOL bOk = AdjustTokenPrivileges(hToken, FALSE, ptpOld, 0, NULL, NULL);
		return ( bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()) );
	}

private:
	std::string					msg_;
	unsigned int				code_;
	_EXCEPTION_POINTERS*		pep_;
};

//
//	结构化异常安装类
//
template<typename SEException = SEExceptionBase>
struct SEExceptionHandler
{
public:
	// 为当前线程安装结构化异常到C++异常的转换器
	static	void				InstallThreadSETranslator()											{ _set_se_translator( TranslateException ); }
	// 为当前进程安装结构化异常处理器
	static	void				InstallUnhandledExceptionHandler()									{ SetUnhandledExceptionFilter( HandleUnhandledException ); }
	// 为当前进程安装结构化异常处理器
	static	void				InstallNewExceptionHandler()										{ std::set_new_handler( HandleNewException ); }
	// 生成Dump文件
	static	void				GenerateDumpFile(_EXCEPTION_POINTERS* ExceptionInfo = NULL)			{ HandleGenerateDumpFile(ExceptionInfo); }

private:
	static void __cdecl			TranslateException(unsigned int code, _EXCEPTION_POINTERS* ExceptionInfo)
	{
		SEException	ex(code, ExceptionInfo);
		SEException::OnThrowException(ex);
		throw	ex;
	}

	static long	WINAPI			HandleUnhandledException( _EXCEPTION_POINTERS *ExceptionInfo )
	{
		SEException	ex(ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo);
		SEException::OnUnhandledException(ex);
		return EXCEPTION_EXECUTE_HANDLER; 
	}

	static void __cdecl			HandleNewException()
	{
		SEException::OnNewException();
	}

	static void __cdecl			HandleGenerateDumpFile(_EXCEPTION_POINTERS* ExceptionInfo)
	{
		_EXCEPTION_POINTERS* pTheExceptionInfo	= ExceptionInfo;
		if(NULL == ExceptionInfo)
		{
			pTheExceptionInfo	= SEException_GetExceptionPointers(5);
		}

		SEException	ex(pTheExceptionInfo->ExceptionRecord->ExceptionCode, pTheExceptionInfo);
		ex.GenerateDumpFile();

		if(NULL == ExceptionInfo)
		{
			SEException_DestroyExceptionPointers(pTheExceptionInfo);
		}
	}

private:
	SEExceptionHandler();
};

#endif
