#pragma once

#include "MsgHandler.h"

class MsgThread : public MsgHandler
{
public:

	MsgThread(); 
	virtual ~MsgThread();

	DWORD		GetThreadId()		const	{ return m_nThreadId; }
	HANDLE		GetThreadHandle()	const	{ return m_hThread; }
	BOOL		IsInMsgThread()		const	{ return (GetCurrentThreadId() == m_nThreadId); }
	BOOL		Start();
	BOOL		Stop(
					const BOOL bKillOnTimeout = FALSE,
					const UINT uTimeout = INFINITE);
	BOOL		Kill();
	BOOL 		PostMessage(
					const UINT		nMsg,
					const WPARAM	wParam,
					const LPARAM	lParam)	{ return PostMessageInternal(nMsg, wParam, lParam, 0);	}
	LRESULT		SendMessage(
					const UINT		nMsg,
					const WPARAM	wParam,
					const LPARAM	lParam,
					const UINT		uTimeout = INFINITE)
											{ return PostMessageInternal(nMsg, wParam, lParam, uTimeout);	}

protected:		
				
	DWORD		m_nThreadId;
	HANDLE		m_hThread;
	HANDLE		m_hEvent;
	HANDLE		m_hIOPort;

private:		

	void		Run();
	BOOL		DispatchMessage(
					const UINT msg,
					const WPARAM wParam,
					const LPARAM lParam);
	BOOL 		PostMessageInternal(
					const UINT nMsg,
					const WPARAM wParam,
					const LPARAM lParam,
					const UINT uTimeout);

	static DWORD WINAPI ThreadFunc(LPVOID lpParameter);
};
