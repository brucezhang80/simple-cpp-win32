// ---------------------------------------------------------------------------------------------------------
//
// ProcessRunner class
// 
// Purpose: Starts a Console Application and captures its stdout and stderr output while the app is running.
//
// Author: ElmüSoft (www.netcult.ch/elmue)
//
// ---------------------------------------------------------------------------------------------------------

/*
----------------------------------------------------------------------------------
Using these conventions results in better readable code and less coding errors !
----------------------------------------------------------------------------------

     cName  for generic class definitions
     CName  for MFC     class definitions
     tName  for type    definitions
     eName  for enum    definitions
     kName  for struct  definitions

    e_Name  for enum variables
    E_Name  for enum constant values

    i_Name  for instances of classes
    h_Name  for handles

    T_Name  for Templates
    t_Name  for char or LPTSTR

    s_Name  for strings
   sa_Name  for Ascii strings
   sw_Name  for Wide (Unicode) strings
   bs_Name  for BSTR
    f_Name  for function pointers
    k_Name  for contructs (struct)

    b_Name  bool,BOOL 1 Bit

   s8_Name    signed  8 Bit (char)
  s16_Name    signed 16 Bit (SHORT, WCHAR)
  s32_Name    signed 32 Bit (LONG, int)
  s64_Name    signed 64 Bit (LONGLONG)

   u8_Name  unsigned  8 Bit (BYTE)
  u16_Name  unsigned 16 bit (WORD)
  u32_Name  unsigned 32 Bit (DWORD, UINT)
  u64_Name  unsigned 64 Bit (ULONGLONG)

    d_Name  for double

  ----------------

    m_Name  for member variables of a class (e.g. ms32_Name for int member variable)
    g_Name  for global (static) variables   (e.g. gu16_Name for global WORD)
    p_Name  for pointer                     (e.g.   ps_Name  *pointer to string)
   pp_Name  for pointer to pointer          (e.g.  ppd_Name **pointer to double)
*/

#include <map>
#include <string>
#include <algorithm>

#include <windows.h>
#include "simple/string.h"

class ProcessRunner  
{
public:
	 ProcessRunner();
	~ProcessRunner();
	std::string Execute(const std::string& s_Commandline, DWORD u32_FirstConvert, const char* t_CurrentDir, const std::string& s_UserVariables, BOOL b_SeparatePipes, DWORD u32_TimeOut, std::string* ps_StdOutput, std::string* ps_StdError, DWORD* pu32_ExitCode);

private:
	void		Init();
	void		CloseHandles();
	std::string CreatePipe(HANDLE* ph_Pipe, HANDLE* ph_File, OVERLAPPED* pk_Overlapped);
	std::string ReadPipe(HANDLE h_Pipe, OVERLAPPED* pk_Overlapped, std::string* ps_Data);
	std::string GetLastErrorMsg();
	std::string CommandLineParamsToOEM(const std::string& s_CommandLine, DWORD u32_FirstConvert);
	int			ZeroStringToMap(const char* t_String, std::map<std::string, std::string> *pi_Map);
	void		MapToZeroString(std::map<std::string, std::string> *pi_Map, char* t_String);
	void		MergeUserEnvironmentStrings(std::string s_UserVar, std::string* ps_Out);

	HANDLE		mh_Process;
	HANDLE		mh_OutPipe;
	HANDLE		mh_ErrPipe;
	HANDLE		mh_OutFile;
	HANDLE		mh_ErrFile;
	OVERLAPPED	mk_OverOut;
	OVERLAPPED	mk_OverErr;
};


DWORD ExecAppEx(
				const char*		t_Comandline,		// IN
				DWORD			u32_FirstConvert,	// IN
				const char*		t_CurrentDir,		// IN
				const char*		t_UserVariables,	// IN
				BOOL			b_SeparatePipes,	// IN
				DWORD			u32_Timeout,		// IN
				std::string*	bs_ApiError,		// OUT
				std::string*	bs_StdOut,			// OUT
				std::string*	bs_StdErr			// OUT
				)
{
	if (!t_Comandline)    t_Comandline    = "";
	if (!t_UserVariables) t_UserVariables = "";

	DWORD u32_ExitCode;
	std::string s_ApiError, s_StdOut, s_StdErr;

	std::string *p_ApiError	= bs_ApiError	?bs_ApiError	: &s_ApiError;
	std::string *p_StdOut	= bs_StdOut		?bs_StdOut		: &s_StdOut;
	std::string *p_StdErr	= bs_StdErr		?bs_StdErr		: &s_StdErr;

	{
		ProcessRunner i_Proc;

		*p_ApiError = i_Proc.Execute(
			t_Comandline, 
			u32_FirstConvert, 
			t_CurrentDir, 
			t_UserVariables, 
			b_SeparatePipes, 
			u32_Timeout, 
			p_StdOut, 
			p_StdErr, 
			&u32_ExitCode
			);
	}

	return u32_ExitCode;
}

// Constructor
ProcessRunner::ProcessRunner()
{
	Init();
}

// Destructor
ProcessRunner::~ProcessRunner()
{
	CloseHandles();
}

void ProcessRunner::Init()
{
	mh_Process = 0;
	mh_OutPipe = 0;
	mh_ErrPipe = 0;
	mh_OutFile = 0;
	mh_ErrFile = 0;

	memset(&mk_OverOut, 0, sizeof(OVERLAPPED));
	memset(&mk_OverErr, 0, sizeof(OVERLAPPED));
}

void ProcessRunner::CloseHandles()
{
	CloseHandle(mh_Process);
	CloseHandle(mh_OutPipe);
	CloseHandle(mh_ErrPipe);
	CloseHandle(mh_OutFile);
	CloseHandle(mh_ErrFile);
}

// Parameter description see ExecuteAW() in CaptureConsole.cpp !
// runs the process which is specified in s_Commandline
// waits until the process has terminated
// fills ps_StdOutput and ps_StdError with the stdout and stderr output of the Console application or batch script
// returns "" if OK, otherwise an error message
std::string ProcessRunner::Execute(
								   const std::string&	s_TheCommandline,	// IN
								   DWORD				u32_FirstConvert,	// IN
								   const char*			t_CurrentDir,		// IN
								   const std::string&   s_UserVariables,	// IN
								   BOOL					b_SeparatePipes,	// IN
								   DWORD				u32_TimeOut,		// IN
								   std::string*			ps_StdOutput,		// OUT
								   std::string*			ps_StdError,		// OUT
								   DWORD*				pu32_ExitCode		// OUT
								   )
{
	// Execute may be called multiple times from the same thread -> Close any open handles
	CloseHandles();
	Init();

	*pu32_ExitCode = 0;
	ps_StdOutput->clear();
	ps_StdError ->clear();

	std::string	s_Commandline(s_TheCommandline);

	std::string s_Err = CreatePipe(&mh_OutPipe, &mh_OutFile, &mk_OverOut);
	if (s_Err.size() > 0)
		return s_Err;

	if (b_SeparatePipes)
	{
		s_Err = CreatePipe(&mh_ErrPipe, &mh_ErrFile, &mk_OverErr);
		if (s_Err.size() > 0)
			return s_Err;
	}

	DWORD u32_Flags = CREATE_NO_WINDOW;
	void* p_Environ = NULL; // Inherit the environment variables from the parent process

	std::string s_Environ;
	if (s_UserVariables.size())
	{
		#ifdef UNICODE
		u32_Flags |= CREATE_UNICODE_ENVIRONMENT;
		#endif

		// Merge the user variables with the environment variables of the parent process
		MergeUserEnvironmentStrings(s_UserVariables, &s_Environ);
		p_Environ = (void*)s_Environ.c_str();
	}

	PROCESS_INFORMATION k_Proc;
	memset(&k_Proc,  0, sizeof(PROCESS_INFORMATION));

	STARTUPINFOA k_Start;
	memset(&k_Start, 0, sizeof(STARTUPINFO));

	k_Start.cb         = sizeof(STARTUPINFO);
	k_Start.hStdError  = (b_SeparatePipes) ? mh_ErrFile : mh_OutFile;
	k_Start.hStdOutput = mh_OutFile;
	k_Start.dwFlags    = STARTF_USESTDHANDLES;

	// Convert special characters in the commandline parameters to DOS codepage
	s_Commandline = CommandLineParamsToOEM(s_Commandline, u32_FirstConvert);

	if (!CreateProcessA(0, const_cast<char*>(s_Commandline.c_str()), 0, 0, TRUE, u32_Flags, p_Environ, t_CurrentDir, &k_Start, &k_Proc))
	{
		return " Error creating Process. " + GetLastErrorMsg();
	}

	if(NULL != mh_Process)
	{
		::CloseHandle(mh_Process);
	}
	mh_Process = k_Proc.hProcess; // closed in destructor

	// wait until the process has exited
	DWORD u32_Elapsed  = 0;
	do
	{
		Sleep(200);
		u32_Elapsed += 200;

		if (!GetExitCodeProcess(k_Proc.hProcess, pu32_ExitCode))
		{
			return " Error getting ExitCode from Process. " + GetLastErrorMsg();
		}
		
		if (u32_TimeOut && u32_Elapsed > u32_TimeOut)
		{
			TerminateProcess(k_Proc.hProcess, 0);
			return " Timeout elapsed.";
		}

		s_Err = ReadPipe(mh_OutPipe, &mk_OverOut, ps_StdOutput);
		if (s_Err.size() > 0)
			return s_Err;

		if (b_SeparatePipes)
		{
			s_Err = ReadPipe(mh_ErrPipe, &mk_OverErr, ps_StdError);
			if (s_Err.size() > 0)
				return s_Err;
		}
	}
	while (*pu32_ExitCode == STILL_ACTIVE);

	return std::string();
}

// Creates a named pipe and a file handle which is used to write to the pipe
std::string ProcessRunner::CreatePipe(HANDLE* ph_Pipe, HANDLE* ph_File, OVERLAPPED* pk_Overlapped)
{
	// Create a threadsafe unique name for the Pipe
	static int s32_Counter = 0;

	std::string s_PipeName;
	{
		char buffer[MAX_PATH];
		_snprintf(buffer, sizeof(buffer), "\\\\.\\pipe\\ProcessRunnerPipe_%X_%X_%X_%X",GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount(), s32_Counter++);
		s_PipeName = buffer;
	}

	SECURITY_ATTRIBUTES k_Secur;
	k_Secur.nLength              = sizeof(SECURITY_ATTRIBUTES);
	k_Secur.lpSecurityDescriptor = 0;
	k_Secur.bInheritHandle       = TRUE;

	*ph_Pipe = CreateNamedPipeA(s_PipeName.c_str(), PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED, 0, 1, 4096, 4096, 0, &k_Secur);

	if (*ph_Pipe == INVALID_HANDLE_VALUE)
	{
		return " Error creating Named Pipe. " + GetLastErrorMsg();
	}

	*ph_File = CreateFileA(s_PipeName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, &k_Secur, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if (*ph_File == INVALID_HANDLE_VALUE)
	{
		return " Error creating Pipe Reader. " + GetLastErrorMsg();
	}

	if (!ConnectNamedPipe(*ph_Pipe, pk_Overlapped))
	{
		if (GetLastError() != ERROR_PIPE_CONNECTED)
		{
			return " Error connecting Pipe. " + GetLastErrorMsg();
		}
	}

	SetHandleInformation(*ph_Pipe, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(*ph_File, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
	return std::string();
}

// Reads from the Pipe and appends the read data to ps_Data
std::string ProcessRunner::ReadPipe(HANDLE h_Pipe, OVERLAPPED* pk_Overlapped, std::string* ps_Data)
{
	// IMPORTANT: Check if there is data that can be read.
	// The first console output will be lost if ReadFile() is called before data becomes available!
	// It does not make any sense but the following 5 lines are indispensable!!
	DWORD u32_Avail = 0;
	if (!PeekNamedPipe(h_Pipe, 0, 0, 0, &u32_Avail, 0))
		return (" Error peeking Pipe. " + GetLastErrorMsg());

	if (!u32_Avail)
		return std::string();

	char s8_Buf[4096];
	DWORD u32_Read = 0;
	do
	{
		if (!ReadFile(h_Pipe, s8_Buf, sizeof(s8_Buf), &u32_Read, pk_Overlapped))
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				return " Error reading Pipe. " + GetLastErrorMsg();
			}
		}

		// ATTENTION: The Console always prints ANSI to the pipe independent if compiled as UNICODE or MBCS!
		s8_Buf[u32_Read] = 0;
		OemToCharA(s8_Buf, s8_Buf); // convert DOS codepage -> ANSI
		*ps_Data += s8_Buf;         // convert ANSI -> Unicode if compiled as Unicode
	}
	while (u32_Read == sizeof(s8_Buf));

	return std::string();
}

// returns the last error as string message
std::string ProcessRunner::GetLastErrorMsg()
{
	DWORD u32_Error = GetLastError();
	if (!u32_Error)
		return std::string();

	std::string s_Err;
	{
		char buffer[MAX_PATH];
		_snprintf(buffer, sizeof(buffer), "API Error %d: ", u32_Error);
		s_Err	= buffer;
	}

	const DWORD BUF_LEN = 2048;
	char t_Msg[BUF_LEN];
	if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, u32_Error, 0, t_Msg, BUF_LEN, 0))
		s_Err += t_Msg;

	return s_Err;
}

// The commandline must be splitted into the application path+filename and the parameters
// The first parameter (path+filename) must NEVER be converted, it may contain Unicode characters
// ONLY the parameters must be converted into the DOS codepage!
// Attention: The filename and the parameters may be enclosed in parenthesis.
// Example:
// "C:\Test\HelloWörld.exe Hello Wörld"  will be converted into
// "C:\Test\HelloWörld.exe Hello W”rld"
// u32_FirstConvert = 0 -> DOS Conversion completely turned off
// u32_FirstConvert = 1 -> convert the first  and all following parameters
// u32_FirstConvert = 2 -> convert the second and all following parameters
std::string ProcessRunner::CommandLineParamsToOEM(const std::string& s_TheCommandLine, DWORD u32_FirstConvert)
{
	std::string	s_CommandLine(s_TheCommandLine);

	// NEVER EVER convert the path+filename!!!
	if (!u32_FirstConvert)
		return s_CommandLine;

	string_ltrim(s_CommandLine);

	BOOL b_Parentesis = FALSE;
	for (int i=0; i<(int)s_CommandLine.size(); i++)
	{
		// Toggle parenthesis flag
		if (s_CommandLine[i] == '\"')
		{
			b_Parentesis = !b_Parentesis;
			continue;
		}

		// Decrement u32_FirstConvert for each space delimited param found in the commandline
		if (s_CommandLine[i] == ' ' && !b_Parentesis)
		{
			// There may be multiple spaces
			while (s_CommandLine[i+1] == ' ')
				i++;

			u32_FirstConvert--;
			continue;
		}

		if (!u32_FirstConvert)
		{
			std::string s_Leave = s_CommandLine.substr(0, i);
			std::string s_Conv  = s_CommandLine.substr(i);

			char* s8_OEM = new char[s_Conv.size()+1];
			
			// If Unicode compiled: Unicode -> DOS codepage
			// If MBCS    compiled: ANSI    -> DOS codepage
			CharToOemA(s_Conv.c_str(), s8_OEM);

			// If Unicode compiled: DOS codepage -> Unicode
			s_CommandLine = s_Leave + s8_OEM;
			
			delete []s8_OEM;
			break;
		}
	}
	return s_CommandLine;
}


// Fills the CMap with the data in the zero terminated string "Key1=Value1\0Key2=Value2\0"
// If the map already contains a key it will be overriden
// returns the length of all added string data
int ProcessRunner::ZeroStringToMap(const char* t_String, std::map<std::string, std::string> *pi_Map)
{
	int s32_TotLen = 0;
	while (true)
	{
		std::string s_String = t_String;
		int  s32_Len = (int)s_String.size();
		if (!s32_Len)
			break;

		size_t s32_Equal = s_String.find('=');
		if (s32_Equal > 0)
		{
			std::string s_Key = s_String.substr(0, s32_Equal);
			std::string s_Val = s_String.substr(s32_Equal+1);

			std::transform(s_Key.begin(), s_Key.end(), s_Key.begin(), _toupper);
			(*pi_Map)[s_Key] = s_Val;
		}

		t_String   += s32_Len +1;
		s32_TotLen += s32_Len +1;
	};
	return s32_TotLen;
}

// Fills the buffer t_String with zero terminated strings "Key1=Value1\0Key2=Value2\0"
// which are obtained from the Map
void ProcessRunner::MapToZeroString(std::map<std::string, std::string> *pi_Map, char* t_String)
{
	std::string s_Key, s_Val;
	std::map<std::string, std::string>::iterator	it = pi_Map->begin();
	for (; it != pi_Map->end(); ++it)
	{
		s_Key	= it->first;
		s_Val	= it->second;

		strcpy(t_String, s_Key.c_str());
		t_String += s_Key.size();

		t_String[0] = '=';
		t_String ++;

		strcpy(t_String, s_Val.c_str());
		t_String += s_Val.size();

		t_String[0] = 0;
		t_String ++;
	}
	t_String[0] = 0;
}

// s_UserVar = "Key1=Value1\nKey2=Value2\n"
// Merges the User Environment Variables with the current System Environment variables
// and returns them in the buffer ps_Out as zero terminated strings
void ProcessRunner::MergeUserEnvironmentStrings(std::string s_UserVar, std::string* ps_Out)
{
#if	defined(GetEnvironmentStrings)
#	undef	GetEnvironmentStrings
#endif
	char* t_Environ = GetEnvironmentStrings();

	std::map<std::string, std::string> i_Map;
	int s32_TotLen = ZeroStringToMap(t_Environ, &i_Map);

	FreeEnvironmentStringsA(t_Environ);

	if (s_UserVar.size())
	{
		if (s_UserVar.substr(s_UserVar.size()-2) != "\n")
			s_UserVar += "\n";

		string_replace(s_UserVar, "\n", "\0");

		s32_TotLen += ZeroStringToMap(s_UserVar.c_str(), &i_Map);
	}

	char* t_Buf = const_cast<char*>(ps_Out->c_str());
	MapToZeroString(&i_Map, t_Buf);
}
