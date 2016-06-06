#ifndef THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	simple thread base class.
//
class	Thread {
public:
    Thread();
    virtual			~Thread();

public:
    bool			start() {
        return	this->do_start();
    }
    bool			stop(bool bKillOnTimeout = false, UINT uTimeout = INFINITE) {
        return	this->do_stop(bKillOnTimeout, uTimeout);
    }
    bool			kill() {
        return	this->do_kill();
    }

public:
    DWORD			thread_id()		const {
        return m_nThreadId;
    }
    HANDLE			thread_handle()	const {
        return m_hThread;
    }
    bool			is_current_thread()		const {
        return (GetCurrentThreadId() == m_nThreadId);
    }

protected:
    virtual	void	do_before_run()			{}
    virtual	void	do_run()				{}
    virtual	void	do_after_run()			{}

    virtual	bool	do_start();
    virtual	bool	do_stop(bool bKillOnTimeout, UINT uTimeout);
    virtual	bool	do_kill();

protected:
    static DWORD WINAPI		thread_entry(LPVOID lpParameter);

protected:
    DWORD		m_nThreadId;
    HANDLE		m_hThread;
};

#endif
