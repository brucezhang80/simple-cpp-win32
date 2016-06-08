
#include	<cassert>

#include	<Windows.h>
#include	<process.h>

#include	"thread.h"

Thread::Thread()
    :
    m_thread_id(0),
    m_thread_handle(NULL),
    m_running(false) {
}

Thread::~Thread() {
    if (m_thread_handle) {
        CloseHandle(m_thread_handle);
        m_thread_handle = NULL;
        m_thread_id	= 0;
    }
}

DWORD	WINAPI	Thread::thread_entry(LPVOID lpParameter) {
    class	StatusGuard {
    public:
        StatusGuard(Thread* thread):thread_(thread) {
            thread_->m_running	= true;
        }
        ~StatusGuard() {
            thread_->m_running	= false;
            if(thread_->event_thread_ended) {
                thread_->event_thread_ended(thread_);
            }
        }
    private:
        Thread*	thread_;
    };

    Thread*	thread	= static_cast<Thread*>(lpParameter);
    {
        StatusGuard	guard(thread);
        if(!thread->do_before_run() ) {
            return	-1;
        }
        thread->do_run();
        thread->do_after_run();
    }

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
