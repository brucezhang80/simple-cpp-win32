
#include	<cassert>

#include	<Windows.h>
#include	<process.h>

#include	"thread.h"

Thread::Thread()
    :
    m_thread_id(0),
    m_thread_handle(NULL) {
}

Thread::~Thread() {
    if (m_thread_handle) {
        CloseHandle(m_thread_handle);
        m_thread_handle = NULL;
        m_thread_id	= 0;
    }
}

DWORD	WINAPI	Thread::thread_entry(LPVOID lpParameter) {
    if(! static_cast<Thread*>(lpParameter)->do_before_run() ) {
        return	-1;
    }
    static_cast<Thread*>(lpParameter)->do_run();
    static_cast<Thread*>(lpParameter)->do_after_run();
    return 0;
}

bool	Thread::do_start() {
    if(NULL != m_thread_handle) {
        return	false;
    }

    m_thread_handle = CreateThread(
                          NULL,
                          0,
                          thread_entry,
                          this,
                          0,
                          &m_thread_id);
    if (!m_thread_handle) {
        return false;
    }

    return true;
}

bool	Thread::do_stop(bool bKillOnTimeout, UINT uTimeout) {
    DWORD nRet = WaitForSingleObject(
                     m_thread_handle,
                     uTimeout);

    if (bKillOnTimeout &&
            WAIT_OBJECT_0 != nRet) {
        kill();
    }

    bool bStopped = (WAIT_OBJECT_0 == nRet);
    if(bStopped) {
        m_thread_handle	= NULL;
        m_thread_id	= 0;
    }

    return bStopped;
}

bool	Thread::do_kill() {
    BOOL bStopped = TerminateThread(
                        m_thread_handle,
                        -1);
    CloseHandle(m_thread_handle);
    m_thread_handle = NULL;
    m_thread_id	= 0;

    return (bStopped != FALSE);
}
