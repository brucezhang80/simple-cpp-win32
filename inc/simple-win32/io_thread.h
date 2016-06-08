#ifndef IO_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define IO_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<deque>
#include	"thread.h"

//
//	simple io thread.
//
struct	package;
class	package_handler;
class	IO_Thread : public Thread {
public:
    IO_Thread();
    virtual				~IO_Thread();

public:
    package*			pop_package();
    void				push_package(package* pkg);
    bool				destroy_package(package* pkg);

protected:
    virtual	bool		do_before_run() {
        m_should_stop	= false;
        return true;
    }
    virtual	void		do_run() {
        while(!m_should_stop) {
        }
    }
    virtual	void		do_after_run()			{}

protected:
    virtual	bool		do_stop(bool bKillOnTimeout, UINT uTimeout) {
        m_should_stop	= true;
        return	Thread::do_stop(bKillOnTimeout, uTimeout);
    }

protected:
    virtual	bool		do_should_stop() {
        return	m_should_stop;
    }

protected:
    package_handler*	get_package_handler();
    void				set_package_handler(package_handler*);

    HANDLE				get_notify_event();
    void				set_notify_event(HANDLE evt);

private:
    class	LockGuard;
    CRITICAL_SECTION		m_io_locker;
    package_handler*		m_pkg_handler;
    std::deque<package*>	m_packages;

    // for subclasses.
    volatile	bool		m_should_stop;
    HANDLE					m_ntf_event;
};

//
//	reader
//
class	IO_ThreadReader : public IO_Thread {
protected:
    virtual	void	do_run();
};

//
//	writer
//
class	IO_ThreadWriter : public IO_Thread {
protected:
    virtual	void	do_run();
};

//
//	Combine Reader and Writer
//
class	IO_ThreadWorker {
public:
    IO_ThreadWorker();
    virtual			~IO_ThreadWorker();

public:
    bool			start() {
        return	this->do_start();
    }
    bool			stop(bool bKillOnTimeout = false, UINT uTimeout = INFINITE) {
        return	this->do_stop(bKillOnTimeout, uTimeout);
    }

public:
    package*		pop_package();
    bool			push_package(package* pkg);
    bool			destroy_package(package* pkg);

protected:
    virtual	bool	do_start();
    virtual	bool	do_stop(bool bKillOnTimeout, UINT uTimeout);

protected:
    IO_ThreadReader*	get_reader();
    void				set_reader(IO_ThreadReader*);

    IO_ThreadWriter*	get_writer();
    void				set_writer(IO_ThreadWriter*);

private:
    void				do_event_thread_ended(Thread*);

private:
    IO_ThreadReader*	m_reader;
    IO_ThreadWriter*	m_writer;
};

#endif
