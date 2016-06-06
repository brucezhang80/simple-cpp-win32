
#include	<cassert>

#include	<Windows.h>
#include	<process.h>

#include	"thread.h"

Thread::Thread()
    :
    m_nThreadId(0),
    m_hThread(NULL) {
}

Thread::~Thread() {
    if (m_hThread) {
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

DWORD	WINAPI	Thread::thread_entry(LPVOID lpParameter) {
    static_cast<Thread*>(lpParameter)->do_before_run();
    static_cast<Thread*>(lpParameter)->do_run();
    static_cast<Thread*>(lpParameter)->do_after_run();
    return 0;
}

bool	Thread::do_start() {
    if(NULL != m_hThread) {
        return	false;
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

    return true;
}

bool	Thread::do_stop(bool bKillOnTimeout, UINT uTimeout) {
    DWORD nRet = WaitForSingleObject(
                     m_hThread,
                     uTimeout);

    if (bKillOnTimeout &&
            WAIT_OBJECT_0 != nRet) {
        kill();
    }

    bool bStopped = (WAIT_OBJECT_0 == nRet);
    if(bStopped) {
        m_hThread	= NULL;
        m_nThreadId	= 0;
    }

    return bStopped;
}

bool	Thread::do_kill() {
    BOOL bStopped = TerminateThread(
                        m_hThread,
                        -1);
    CloseHandle(m_hThread);
    m_hThread = NULL;

    return (bStopped != FALSE);
}
