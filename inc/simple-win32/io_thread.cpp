
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

void	IO_Reader::do_run() {
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

void	IO_Writer::do_run() {
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
