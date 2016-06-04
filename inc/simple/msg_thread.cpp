
#include	<cassert>

#include	<Windows.h>
#include	<process.h>

#include	"msg_handler.h"
#include	"msg_thread.h"

class LParamI {
public:

    LParamI(bool bCreateEvent, LPARAM lParam)
        :
        m_hEvent(NULL),
        m_lParam(lParam) {
        if (bCreateEvent) {
            m_hEvent = CreateEvent(
                           NULL,
                           false,
                           false,
                           NULL);
        }
    }

    ~LParamI() {
        if (m_hEvent) {
            CloseHandle(m_hEvent);
            m_hEvent = NULL;
        }
    }

    const HANDLE	GetEvent()	const {
        return m_hEvent;
    }
    LPARAM			GetLParam() {
        return m_lParam;
    }

private:

    HANDLE	m_hEvent;
    LPARAM	m_lParam;
};

Msg_Thread::Msg_Thread()
    :
    m_nThreadId(0),
    m_hThread(NULL),
    m_hEvent(NULL),
    m_hIOPort(NULL),
    m_pHandler(NULL) {
}

Msg_Thread::~Msg_Thread() {
    if (m_hEvent) {
        CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }

    if (m_hIOPort) {
        CloseHandle(m_hIOPort);
        m_hIOPort = NULL;
    }

    if (m_hThread) {
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

DWORD	WINAPI	Msg_Thread::thread_entry(LPVOID lpParameter) {
    static_cast<Msg_Thread*>(lpParameter)->do_before_run();
    static_cast<Msg_Thread*>(lpParameter)->do_run();
    static_cast<Msg_Thread*>(lpParameter)->do_after_run();
    return 0;
}

bool	Msg_Thread::start(Msg_Handler* pHandler) {
    if(NULL == pHandler) {
        return	false;
    }
    if(NULL != m_hThread) {
        return	false;
    }

    m_pHandler	= pHandler;
    if (!m_hEvent) {
        m_hEvent = CreateEvent(
                       NULL,
                       false,
                       false,
                       NULL);
        if (!m_hEvent) {
            return false;
        }
    }

    m_hThread = CreateThread(
                    NULL,
                    0,
                    thread_entry,
                    this,
                    0,
                    &m_nThreadId);
    if (!m_hThread) {
        return false;
    }

    DWORD nRet = WaitForSingleObject(
                     m_hEvent,
                     INFINITE);

    return (WAIT_OBJECT_0 == nRet);
}

bool	Msg_Thread::stop(const bool bKillOnTimeout, const UINT uTimeout) {
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
            WAIT_OBJECT_0 != nRet) {
        kill();
    }

    bool bStopped = (WAIT_OBJECT_0 == nRet);
    if(bStopped) {
        m_hThread	= NULL;
        m_nThreadId	= 0;
        if (m_hEvent) {
            CloseHandle(m_hEvent);
            m_hEvent = NULL;
        }
    }

    return bStopped;
}

bool	Msg_Thread::kill() {
    BOOL bStopped = TerminateThread(
                        m_hThread,
                        0);
    CloseHandle(m_hThread);
    m_hThread = NULL;

    return (bStopped != FALSE);
}

void	Msg_Thread::do_run() {
    m_hIOPort = CreateIoCompletionPort(
                    INVALID_HANDLE_VALUE,
                    NULL,
                    0,
                    1);
    SetEvent(m_hEvent);
    if (!m_hIOPort) {
        return;
    }

    while (true) {
        try {
            DWORD nBytesRead = 0;
            ULONG_PTR nKey = 0;
            LPOVERLAPPED lpOverlapped = NULL;
            BOOL bRet = GetQueuedCompletionStatus(
                            m_hIOPort,
                            &nBytesRead,
                            &nKey,
                            &lpOverlapped,
                            INFINITE);
            if (!bRet) {
                break;
            }

            if (0 == nKey) {
                break;
            }

            PMSG pMSG = (PMSG) nKey;
            do_dispatch_msg(
                pMSG->message,
                pMSG->wParam,
                pMSG->lParam);

            delete pMSG;
            pMSG = NULL;
        } catch(...) {
            /// ??? log???
        }
    }

    SetEvent(m_hEvent);
}

bool	Msg_Thread::do_dispatch_msg(const UINT nMsg, const WPARAM wParam, const LPARAM lParam) {
    LParamI* lParamI = (LParamI*) lParam;

    bool bRet = false;
    if(NULL != m_pHandler) {
        bRet	= m_pHandler->handle_msg(
                      nMsg,
                      wParam,
                      lParamI->GetLParam());
    }

    if (lParamI->GetEvent()) {
        SetEvent(lParamI->GetEvent());
    } else {
        delete lParamI;
        lParamI = NULL;
    }

    return bRet;
}

bool	Msg_Thread::do_post_msg(
    const UINT		nMsg,
    const WPARAM	wParam,
    const LPARAM	lParam,
    const UINT		uTimeout
) {
    if(NULL == m_hThread) {
        return false;
    }

    const bool bCreateEvent = ((uTimeout > 0) && !is_in_msg_thread());
    const bool bDispatch = ((uTimeout > 0) && is_in_msg_thread());

    LParamI* lParamI = new LParamI(
        bCreateEvent,
        lParam);

    if (bCreateEvent	&&
            !lParamI->GetEvent()) {
        delete lParamI;
        lParamI = NULL;
        return false;
    }

    BOOL bRet = FALSE;

    if (bDispatch) {
        bRet = do_dispatch_msg(
                   nMsg,
                   wParam,
                   (LPARAM) lParamI);
    } else {
        MSG* pMSG = new MSG;
        pMSG->hwnd = NULL;
        pMSG->message = nMsg;
        pMSG->wParam = wParam;
        pMSG->lParam = (LPARAM) lParamI;

        bRet = PostQueuedCompletionStatus(
                   m_hIOPort,
                   0,
                   ULONG_PTR(pMSG),
                   NULL);
        if (!bRet) {
            delete lParamI;
            lParamI = NULL;
            delete pMSG;
            pMSG = NULL;
            return false;
        }
    }

    if (bCreateEvent && lParamI->GetEvent()) {
        DWORD dwRet = WaitForSingleObject(
                          lParamI->GetEvent(),
                          uTimeout);
        if (WAIT_OBJECT_0 == dwRet) {
            delete lParamI;
            lParamI = NULL;
        } else {
            // Cant avoid this leak!
            assert(WAIT_OBJECT_0 == dwRet);
            bRet = FALSE;
        }
    }

    return (bRet != FALSE);
}
