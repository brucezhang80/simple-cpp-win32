
#include	<cassert>

#include	<Windows.h>

#include	"simple/package.h"
#include	"io_thread.h"

class	IO_Thread::LockGuard {
public:
    LockGuard(CRITICAL_SECTION& locker) : m_Locker(locker) {
        EnterCriticalSection(&m_Locker);
    }
    ~LockGuard() {
        LeaveCriticalSection(&m_Locker);
    }

private:
    CRITICAL_SECTION&	m_Locker;
};

IO_Thread::IO_Thread()
    : m_pkg_handler(NULL)
    , m_should_stop(false)
    , m_ntf_event(NULL) {
    ::InitializeCriticalSection(&m_io_locker);
}

IO_Thread::~IO_Thread() {
    ::DeleteCriticalSection(&m_io_locker);
}

package_handler*	IO_Thread::get_package_handler() {
    return	m_pkg_handler;
}

void	IO_Thread::set_package_handler(package_handler* handler) {
    m_pkg_handler	= handler;
}

HANDLE	IO_Thread::get_notify_event() {
    return	m_ntf_event;
}

void	IO_Thread::set_notify_event(HANDLE evt) {
    m_ntf_event	= evt;
}

package* IO_Thread::pop_package() {
    LockGuard	guard(m_io_locker);
    if(m_packages.empty()) {
        return	NULL;
    } else {
        package*	pkg	= m_packages.front();
        m_packages.pop_front();

        return	pkg;
    }
}

void IO_Thread::push_package(package* pkg) {
    if(NULL != pkg) {
        LockGuard	guard(m_io_locker);
        m_packages.push_back(pkg);
    }
    if(NULL != pkg && NULL != m_ntf_event) {
        SetEvent(m_ntf_event);
    }
}

bool IO_Thread::destroy_package(package* pkg) {
    if(NULL == m_pkg_handler) {
        return	false;
    }

    m_pkg_handler->destroy_package(pkg);
    return	true;
}

void	IO_ThreadReader::do_run() {
    package_handler*	pkg_handler	= this->get_package_handler();
    if(NULL == pkg_handler) {
        return;
    }

    package*	pkg	= NULL;
    while(!this->do_should_stop()) {
        // 接收
        try {
            if(NULL == pkg) {
                pkg	= pkg_handler->create_package(0, NULL, 0, 0);
            }

            int	nRet	= pkg_handler->read_package(pkg);

            if(nRet < 0) {
                break;
            }

            if(0 == nRet) {
                push_package(pkg);
            }
        } catch(...) {
            // log ???
        }
    }

    if(NULL != pkg) {
        pkg_handler->destroy_package(pkg);
    }
}

void	IO_ThreadWriter::do_run() {
    package_handler*	pkg_handler	= this->get_package_handler();
    if(NULL == pkg_handler) {
        return;
    }

    package*	pkg	= NULL;
    while(!this->do_should_stop()) {
        // 发送
        try {
            if(NULL == pkg) {
                pkg	= this->pop_package();
                if(NULL == pkg) {
                    HANDLE evt	= get_notify_event();
                    if(NULL != evt) {
                        WaitForSingleObject(evt, 500);
                    } else {
                        SwitchToThread();
                    }
                }
            }

            if(NULL != pkg) {
                int	nRet	= pkg_handler->write_package(pkg);

                if(nRet < 0) {
                    break;
                }

                if(0 == nRet) {
                    pkg_handler->destroy_package(pkg);
                    pkg	= NULL;
                } else {
                    SwitchToThread();
                }
            }
        } catch(...) {
            // log ???
        }
    }

    if(NULL != pkg) {
        pkg_handler->destroy_package(pkg);
    }
}

IO_ThreadWorker::IO_ThreadWorker()
    :	m_reader(NULL)
    ,	m_writer(NULL) {
}

IO_ThreadWorker::~IO_ThreadWorker() {
    if(NULL != m_reader) {
        if(m_reader->is_running()) {
            m_reader->stop(true, INFINITE);
        }
        delete	m_reader;
        m_reader	= NULL;
    }
    if(NULL != m_writer) {
        if(m_writer->is_running()) {
            m_writer->stop(true, INFINITE);
        }
        delete	m_writer;
        m_writer	= NULL;
    }
}

bool	IO_ThreadWorker::do_start() {
    if(NULL == m_reader || NULL == m_writer) {
        return	false;
    }

    m_reader->event_thread_ended.bind(this, &IO_ThreadWorker::do_event_thread_ended);
    m_writer->event_thread_ended.bind(this, &IO_ThreadWorker::do_event_thread_ended);

    if(!m_writer->start()) {
        return	false;
    }
    if(!m_reader->start()) {
        m_writer->stop();
        return	false;
    }

    return	true;
}

bool	IO_ThreadWorker::do_stop(bool bKillOnTimeout, UINT uTimeout) {
    if(NULL == m_reader || NULL == m_writer) {
        return	false;
    }

    m_reader->stop(bKillOnTimeout, uTimeout);
    m_writer->stop(bKillOnTimeout, uTimeout);
    return	true;
}

package*	IO_ThreadWorker::pop_package() {
    if(NULL == m_reader) {
        return	NULL;
    }

    return	m_reader->pop_package();
}

bool	IO_ThreadWorker::push_package(package* pkg) {
    if(NULL == m_writer) {
        return	false;
    }

    m_writer->push_package(pkg);
    return	true;
}

bool	IO_ThreadWorker::destroy_package(package* pkg) {
    if(NULL == m_writer) {
        return	m_writer->destroy_package(pkg);
    }
    if(NULL == m_reader) {
        return	m_reader->destroy_package(pkg);
    }
}

IO_ThreadReader*	IO_ThreadWorker::get_reader() {
    return	m_reader;
}

void	IO_ThreadWorker::set_reader(IO_ThreadReader* reader) {
    m_reader	= reader;
}

IO_ThreadWriter*	IO_ThreadWorker::get_writer() {
    return	 m_writer;
}

void	IO_ThreadWorker::set_writer(IO_ThreadWriter* writer) {
    m_writer	= writer;
}

void	IO_ThreadWorker::do_event_thread_ended(Thread* thread) {
    if(thread != m_reader && m_reader != NULL) {
        m_reader->stop();
    }
    if(thread != m_reader && m_writer != NULL) {
        m_writer->stop();
    }
}
