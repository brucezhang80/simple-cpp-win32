#include "MsgThread.h"
#include <process.h>

class LParamI
{
public:

	LParamI(BOOL bCreateEvent, LPARAM lParam)
		:
		m_hEvent(NULL),
		m_lParam(lParam)
	{
		if (bCreateEvent)
		{
			m_hEvent = CreateEvent(
				NULL,
				FALSE,
				FALSE,
				NULL);
		}
	}

	~LParamI()
	{
		if (m_hEvent)
		{
			CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	}

	const HANDLE	GetEvent()	const	{ return m_hEvent; }
	LPARAM			GetLParam()			{ return m_lParam; }

private:

	HANDLE	m_hEvent;
	LPARAM	m_lParam;
};

MsgThread::MsgThread()
	:
	m_nThreadId(0),
	m_hThread(NULL),
	m_hEvent(NULL),
	m_hIOPort(NULL)
{
}

MsgThread::~MsgThread()
{
	if (m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	if (m_hIOPort)
	{
		CloseHandle(m_hIOPort);
		m_hIOPort = NULL;
	}

	if (m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

DWORD
WINAPI
MsgThread::ThreadFunc
(
	LPVOID lpParameter
)
{
	static_cast<MsgThread*>(lpParameter)->Run();
	return 0;
}

BOOL
MsgThread::Start()
{
	if (m_hThread)
	{
		return TRUE;
	}

	if (!m_hEvent)
	{
		m_hEvent = CreateEvent(
			NULL,
			FALSE,
			FALSE,
			NULL);
		if (!m_hEvent)
		{
			return FALSE;
		}
	}
	
	m_hThread = CreateThread(
		NULL,
		0,
		ThreadFunc,
		this,
		0,
		&m_nThreadId);
	if (!m_hThread)
	{
		return FALSE;
	}

	DWORD nRet = WaitForSingleObject(
		m_hEvent,
		INFINITE);

	return (WAIT_OBJECT_0 == nRet);
}

BOOL
MsgThread::Stop
(
	const BOOL bKillOnTimeout,
	const UINT uTimeout
)
{
	PostQueuedCompletionStatus(
		m_hIOPort,
		0,
		0,
		NULL);

	DWORD nRet = WaitForSingleObject(
		m_hEvent,
		uTimeout);
	SetEvent(m_hEvent);

	if (bKillOnTimeout &&
		WAIT_OBJECT_0 != nRet)
	{
		Kill();
	}

	return (WAIT_OBJECT_0 == nRet);
}

BOOL
MsgThread::Kill()
{
	BOOL bRet = TerminateThread(
		m_hThread,
		0);
	CloseHandle(m_hThread);
	m_hThread = NULL;

	return bRet;
}

void
MsgThread::Run()
{
	m_hIOPort = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		1);
	SetEvent(m_hEvent);
	if (!m_hIOPort)
	{
		return;
	}

	while (true)
	{
		DWORD nBytesRead = 0;
		ULONG_PTR nKey = 0;
		LPOVERLAPPED lpOverlapped = NULL;
		BOOL bRet = GetQueuedCompletionStatus(
			m_hIOPort,
			&nBytesRead,
			&nKey,
			&lpOverlapped,
			INFINITE);
		if (!bRet)
		{
			SetEvent(m_hEvent);
			break;
		}

		if (0 == nKey)
		{
			SetEvent(m_hEvent);
			break;
		}

		PMSG pMSG = (PMSG) nKey;

		DispatchMessage(
			pMSG->message,
			pMSG->wParam,
			pMSG->lParam);

		delete pMSG;
		pMSG = NULL;
	}
}

BOOL
MsgThread::DispatchMessage
(
	const UINT		nMsg,
	const WPARAM	wParam,
	const LPARAM	lParam
)
{
	LParamI* lParamI = (LParamI*) lParam;

	BOOL bRet = OnMsg(
		nMsg,
		wParam,
		lParamI->GetLParam());

	if (lParamI->GetEvent())
	{
		SetEvent(lParamI->GetEvent());
	}
	else
	{
		delete lParamI;
		lParamI = NULL;
	}

	return bRet;
}

BOOL
MsgThread::PostMessageInternal
(
	const UINT		nMsg,
	const WPARAM	wParam,
	const LPARAM	lParam,
	const UINT		uTimeout
)
{
	const BOOL bCreateEvent = ((uTimeout > 0) && !IsInMsgThread());
	const BOOL bDispatch = ((uTimeout > 0) && IsInMsgThread());

	LParamI* lParamI = new LParamI(
		bCreateEvent,
		lParam);

	if (bCreateEvent	&&
		!lParamI->GetEvent())
	{
		delete lParamI;
		lParamI = NULL;
		return FALSE;
	}

	BOOL bRet = FALSE;

	if (bDispatch)
	{
		bRet = DispatchMessage(
			nMsg,
			wParam,
			(LPARAM) lParamI);
	}
	else
	{
		MSG* pMSG = new MSG;
		pMSG->hwnd = NULL;
		pMSG->message = nMsg;
		pMSG->wParam = wParam;
		pMSG->lParam = (LPARAM) lParamI;

		bRet = PostQueuedCompletionStatus(
			m_hIOPort,
			0,
			(DWORD) pMSG,
			NULL);
		if (!bRet)
		{
			delete lParamI;
			lParamI = NULL;
			delete pMSG;
			pMSG = NULL;
			return FALSE;
		}
	}

	if (lParamI->GetEvent())
	{
		DWORD dwRet = WaitForSingleObject(
			lParamI->GetEvent(),
			uTimeout);
		if (WAIT_OBJECT_0 == dwRet)
		{
			delete lParamI;
			lParamI = NULL;
		}
		else
		{
			// Cant avoid this leak!

			bRet = FALSE;
		}
	}

	return bRet;
}
