#ifndef MSG_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define MSG_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

class	Msg_Handler;
class	Msg_Thread {
public:
    Msg_Thread();
    virtual			~Msg_Thread();

public:
    bool			start(Msg_Handler* pHandler);
    bool			stop(const bool bKillOnTimeout = false, const UINT uTimeout = INFINITE);
    bool			kill();

public:
    DWORD			thread_id()		const {
        return m_nThreadId;
    }
    HANDLE			thread_handle()	const {
        return m_hThread;
    }
    bool			is_in_msg_thread()		const {
        return (GetCurrentThreadId() == m_nThreadId);
    }

public:
    bool 			post_msg(const UINT nMsg,const WPARAM wParam,const LPARAM lParam) {
        return do_post_msg(nMsg, wParam, lParam, 0);
    }
    LRESULT			send_msg(const UINT nMsg, const WPARAM wParam, const LPARAM lParam, const UINT uTimeout = INFINITE) {
        return do_post_msg(nMsg, wParam, lParam, uTimeout);
    }

protected:
    virtual	void	do_run();
    virtual	void	do_before_run()			{}
    virtual	void	do_after_run()			{}

private:
    bool			do_dispatch_msg(const UINT msg, const WPARAM wParam, const LPARAM lParam);
    bool 			do_post_msg(const UINT nMsg, const WPARAM wParam, const LPARAM lParam, const UINT uTimeout);

    static DWORD WINAPI		thread_entry(LPVOID lpParameter);

protected:
    DWORD		m_nThreadId;
    HANDLE		m_hThread;
    HANDLE		m_hEvent;
    HANDLE		m_hIOPort;
    Msg_Handler*m_pHandler;
};

#endif
