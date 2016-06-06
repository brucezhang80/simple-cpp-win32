#ifndef ANTI_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define ANTI_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<psapi.h>
#include	"_third/AntiRE.h"

// CheckForCCBreakpoint check if there's a CC (INT3) breakpoint
inline bool Anti_CheckForCCBreakpoint(void* pMemory,  size_t SizeToCheck) {
    return	CheckForCCBreakpoint(pMemory, SizeToCheck);
}

// Check if there's a memory break point. OllyDBG
inline bool Anti_CheckMemoryBreakpointDebugger() {
    return	CheckMemoryBreakpointDebugger();
}

// CheckHardwareBreakpoints returns the number of hardware
// breakpoints detected and on failure it returns -1.
inline int Anti_CheckHardwareBreakpoints() {
    return	CheckHardwareBreakpoints();
}

// CheckCloseHandle will call CloseHandle on an invalid
// DWORD aligned value and if a debugger is running an exception
// will occur and the function will return true otherwise it'll
// return false
inline bool Anti_CheckDbgPresentCloseHandle() {
    return	CheckDbgPresentCloseHandle();
}

// OllyDBG will crash.
//OutputDebugString( TEXT("%s%s%s%s%s%s%s%s%s%s%s")
//                TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s")
//                TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s")
//                TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s") );

#define ANTI_JUNK_CODE_SIMPLE	JUNK_CODE_SIMPLE

// This function will erase the current images
// PE header from memory preventing a successful image
// if dumped
inline void Anti_ErasePEHeaderFromMemory() {
    ErasePEHeaderFromMemory();
}

// This function uses the toolhelp32 api to enumerate all running processes
// on the computer and does a comparison of the process name against the
// ProcessName parameter. The function will return 0 on failure.
inline DWORD Anti_GetProcessIdFromName(LPCTSTR ProcessName) {
    return GetProcessIdFromName(ProcessName);
}

// This function will return the process id of csrss.exe
// and will do so in two different ways. If the OS is XP or
// greater NtDll has a CsrGetProcessId otherwise I'll use
// GetProcessIdFromName. Like other functions it will
// return 0 on failure.
inline DWORD Anti_GetCsrssProcessId() {
    return GetCsrssProcessId();
}

// This function will return the process id of Explorer.exe by using the
// GetShellWindow function and the GetWindowThreadProcessId function
inline DWORD Anti_GetExplorerPIDbyShellWindow() {
    return GetExplorerPIDbyShellWindow();
}

// GetParentProcessId will use the NtQueryInformationProcess function
// exported by NtDll to retrieve the parent process id for the current
// process and if for some reason it doesn't work, it returns 0
inline DWORD Anti_GetParentProcessId() {
    return GetParentProcessId();
}

// The function will attempt to open csrss.exe with
// PROCESS_ALL_ACCESS rights if it fails we're
// not being debugged however, if its successful we probably are
inline bool Anti_CanOpenCsrss() {
    return CanOpenCsrss();
}

// This function returns true if the parent process of
// the current running process is Explorer.exe
inline bool Anti_IsParentExplorerExe() {
    return IsParentExplorerExe();
}

// Debug self is a function that uses CreateProcess
// to create an identical copy of the current process
// and debugs it
inline void Anti_DebugSelf() {
    DebugSelf();
}

// HideThread will attempt to use
// NtSetInformationThread to hide a thread
// from the debugger, this is essentially the same
// as HideThreadFromDebugger. Passing NULL for
// hThread will cause the function to hide the thread
// the function is running in. Also, the function returns
// false on failure and true on success
inline bool Anti_HideThread(HANDLE hThread) {
    return HideThread(hThread);
}


// This function uses NtQuerySystemInformation
// to try to retrieve a handle to the current
// process's debug object handle. If the function
// is successful it'll return true which means we're
// being debugged or it'll return false if it fails
// or the process isn't being debugged
inline bool Anti_CheckDebugObject() {
    return CheckDebugObject();
}

// CheckProcessDebugFlags will return true if
// the EPROCESS->NoDebugInherit is == FALSE,
// the reason we check for false is because
// the NtQueryProcessInformation function returns the
// inverse of EPROCESS->NoDebugInherit so (!TRUE == FALSE)
inline bool Anti_CheckProcessDebugFlags() {
    return CheckProcessDebugFlags();
}


// CheckOutputDebugString checks whether or
// OutputDebugString causes an error to occur
// and if the error does occur then we know
// there's no debugger, otherwise if there IS
// a debugger no error will occur
inline bool Anti_CheckOutputDebugString(LPCTSTR String) {
    return CheckOutputDebugString(String);
}

// The Int2DCheck function will check to see if a debugger
// is attached to the current process. It does this by setting up
// SEH and using the Int 2D instruction which will only cause an
// exception if there is no debugger. Also when used in OllyDBG
// it will skip a byte in the disassembly and will create
// some havoc.
inline bool Anti_Int2DCheck() {
    return Int2DCheck();
}


// ObjectListCheck uses NtQueryObject to check the environments
// list of objects and more specifically for the number of
// debug objects. This function can cause an exception (although rarely)
// so either surround it in a try catch or __try __except block
// but that shouldn't happen unless one tinkers with the function
inline bool Anti_ObjectListCheck() {
    return ObjectListCheck();
}


// The IsDbgPresentPrefixCheck works in at least two debuggers
// OllyDBG and VS 2008, by utilizing the way the debuggers handle
// prefixes we can determine their presence. Specifically if this code
// is ran under a debugger it will simply be stepped over;
// however, if there is no debugger SEH will fire :D
inline bool Anti_IsDbgPresentPrefixCheck() {
    return IsDbgPresentPrefixCheck();
}

//
//	Is run in Virtual Machine or not.
//
inline bool	Anti_IsInVMMatrix() {
    return	IsInVMMatrix();
}

inline bool	Anti_IsParentSameImage() {
    return IsParentSameImage();
}

#endif
