#ifndef MSG_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define MSG_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	"thread.h"

class	Msg_Handler;
class	Msg_Thread : public Thread {
public:
    Msg_Thread(Msg_Handler* pHandler);
    virtual			~Msg_Thread();

protected:
    virtual	bool	do_start();
    virtual	bool	do_stop(bool bKillOnTimeout, UINT uTimeout);

public:
    bool 			post_msg(const UINT nMsg,const WPARAM wParam,const LPARAM lParam) {
        return do_post_msg(nMsg, wParam, lParam, 0);
    }
    LRESULT			send_msg(const UINT nMsg, const WPARAM wParam, const LPARAM lParam, const UINT uTimeout = INFINITE) {
        return do_post_msg(nMsg, wParam, lParam, uTimeout);
    }

protected:
    virtual	void	do_run();

    void			do_set_msg_handler(Msg_Handler* pHandler) {
        m_pHandler	= pHandler;
    }

private:
    bool			do_dispatch_msg(const UINT msg, const WPARAM wParam, const LPARAM lParam);
    bool 			do_post_msg(const UINT nMsg, const WPARAM wParam, const LPARAM lParam, const UINT uTimeout);

protected:
    HANDLE		m_hEvent;
    HANDLE		m_hIOPort;
    Msg_Handler*m_pHandler;
};

#endif
