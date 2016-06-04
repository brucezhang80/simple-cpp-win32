#ifndef FIBER_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define FIBER_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include "simple/delegate.h"
#include "msg_thread.h"

//
//	纤程上下文
//
struct	Fiber_Context {
    virtual	void		wait(float dt)	= 0;
    virtual	void		stop()			= 0;
    virtual	void		yield()			= 0;
};

//
//	纤程执行器
//
class Fiber_Executor : public Msg_Thread {
public:
    Fiber_Executor(void);
    ~Fiber_Executor(void);

public:
    enum			{ INVALID_FIBER_SCRIPT_ID = 0 };
    typedef			delegate<void(Fiber_Context* ctx, void* param)>		FiberScript;
    typedef			delegate<void(unsigned long id, void* param)>		FiberScriptEndCallbacker;

public:
    unsigned long	add_script(FiberScript script, FiberScriptEndCallbacker callbacker, LPVOID param, bool wait = false, DWORD stack_size = 0);
    bool			stop_script(int script_id);
    bool			is_script_running(int script_id);

    size_t			get_scripts_sum();
    void			clear_scripts();

    bool			update(double dt, bool wait = false);

protected:
    virtual void	do_before_run();
    virtual void	do_after_run();

private:
    struct	Impl;
    Impl*			impl_;
};

#endif
