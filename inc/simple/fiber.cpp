
#include	<deque>

#include	<Windows.h>

#include	"fiber.h"

#include	"msg_handler.h"

//
//	Fiber_Context实现
//
class FiberContextImpl : public Fiber_Context {
public:
    FiberContextImpl()
        :	id_(0)
        ,	running_(true)
        ,	param_(NULL)
        ,	stack_size_(0)
        ,	fiber_(NULL)
        ,	main_fiber_(NULL)
        ,	timestamp_(0) {
    }

public:
    virtual	void		wait(float dt) {
        const double	beginTime	= timestamp_;
        volatile double	currTime	= timestamp_;
        while(currTime - beginTime < dt) {
            this->yield();
            currTime	= timestamp_;
        }
    }
    virtual	void		stop() {
        running_	= false;
        this->yield();
    }
    virtual	void		yield() {
        if(NULL != main_fiber_) {
            SwitchToFiber(main_fiber_);
        }
    }

    bool				is_running() {
        return running_;
    }

public:
    int					id_;
    void*				param_;
    bool				running_;
    void*				fiber_;
    DWORD				stack_size_;
    volatile double		timestamp_;
    void*				main_fiber_;

    Fiber_Executor::FiberScript					script_;
    Fiber_Executor::FiberScriptEndCallbacker	callbacker_;
};


struct	Fiber_Executor::Impl : Msg_Handler {
    Impl() : id_seed_(0)
        , main_fiber_(0)
        , timestamp_(0) {
    }

    unsigned long	id_seed_;
    LPVOID			main_fiber_;
    volatile double	timestamp_;

    typedef			std::deque<Fiber_Context*>		SCRIPT_LIST;
    SCRIPT_LIST		scripts_;

    struct	FiberScriptEndHandler {
        int							id;
        void*						param;
        Fiber_Executor::FiberScriptEndCallbacker	handler;
    };
    typedef			std::deque<FiberScriptEndHandler>	SCRIPT_END_HANDLER_LIST;
    SCRIPT_END_HANDLER_LIST	end_handlers_;

    enum OPERATION_TYPE {
        OPERATION_NONE,
        OPERATION_EXECUTE_LOGIC,
        OPERATION_UPDATE_LOGIC,
        OPERATION_DELETE_LOGIC,
        OPERATION_CLEAR_ALL,
        OPERATION_TYPE_SUM,
    };

    MSG_HANDLER_BEGIN()
    MSG_HANDLER(OPERATION_EXECUTE_LOGIC,		&Impl::do_execute_logic)
    MSG_HANDLER(OPERATION_UPDATE_LOGIC,			&Impl::do_update_logic)
    MSG_HANDLER(OPERATION_DELETE_LOGIC,			&Impl::do_delete_logic)
    MSG_HANDLER(OPERATION_CLEAR_ALL,			&Impl::do_clear_logic)
    MSG_HANDLER_END()

    void do_execute_logic(WPARAM wParam, LPARAM lParam) {
        FiberContextImpl*	pScript	= (FiberContextImpl*)lParam;
        if(NULL != pScript) {
            pScript->fiber_			= CreateFiber(pScript->stack_size_, &Impl::Fiber_Script_Excuter_Thread_Func, pScript);
            pScript->main_fiber_	= this->main_fiber_;
            scripts_.push_back(pScript);
        }
    }

    void do_update_logic(WPARAM wParam, LPARAM lParam) {
        {
            SCRIPT_LIST::iterator	it		= scripts_.begin();
            for(; it != scripts_.end(); ++it) {
                FiberContextImpl*	pScript = static_cast<FiberContextImpl*>(*it);
                if(pScript->is_running()) {
                    pScript->timestamp_	= timestamp_;
                    SwitchToFiber(pScript->fiber_);
                }
            }
        }

        {
            SCRIPT_LIST::iterator	it		= scripts_.begin();
            for(; it != scripts_.end(); ) {
                FiberContextImpl*	pScript = static_cast<FiberContextImpl*>(*it);
                if(!pScript->is_running()) {
                    if(pScript->callbacker_) {
                        FiberScriptEndHandler handler;
                        handler.id				= pScript->id_;
                        handler.param			= pScript->param_;
                        handler.handler			= pScript->callbacker_;
                        end_handlers_.push_back(handler);
                    }

                    DeleteFiber(pScript->fiber_);
                    delete *it;
                    it = scripts_.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    void do_delete_logic(WPARAM wParam, LPARAM lParam) {
        SCRIPT_LIST::iterator	it		= scripts_.begin();
        SCRIPT_LIST::iterator	it_end	= scripts_.end();

        for(; it != it_end; ++it) {
            FiberContextImpl*	pScript = static_cast<FiberContextImpl*>(*it);
            if(pScript->id_ == lParam) {
                DeleteFiber(pScript->fiber_);
                delete *it;
                scripts_.erase(it);
                break;
            }
        }
    }

    void do_clear_logic(WPARAM wParam, LPARAM lParam) {
        std::deque<int>	script_ids;

        {
            SCRIPT_LIST::iterator	it		= scripts_.begin();
            SCRIPT_LIST::iterator	it_end	= scripts_.end();
            for(; it != it_end; ++it) {
                FiberContextImpl*	pScript = static_cast<FiberContextImpl*>(*it);
                script_ids.push_back(pScript->id_);
            }
        }

        {
            std::deque<int>::const_iterator	it		= script_ids.begin();
            std::deque<int>::const_iterator	it_end	= script_ids.end();
            for(; it != it_end; ++it) {
                do_delete_logic(0, *it);
            }
        }
    }

    static void WINAPI	Fiber_Script_Excuter_Thread_Func(LPVOID lParam) {
        FiberContextImpl*		pScript	= (FiberContextImpl*)lParam;
        if(pScript->script_) {
#if	defined(NDEBUG)
            try
#endif
            {
                pScript->script_(pScript, pScript->param_);
            }
#if	defined(NDEBUG)
            catch(...) {
            }
#endif
        }
        pScript->stop();
    }

};

//
//
//
Fiber_Executor::Fiber_Executor(void)
    :	impl_(new Impl()) {
}

Fiber_Executor::~Fiber_Executor(void) {
    stop(TRUE, 1000);

    delete	impl_;
    impl_	= NULL;
}

unsigned long Fiber_Executor::add_script(FiberScript script, FiberScriptEndCallbacker callbacker, LPVOID param, bool wait, DWORD stack_size) {
    while(++impl_->id_seed_ == INVALID_FIBER_SCRIPT_ID) {
        // TODO: ... ID重复
    }

    int	script_id		= impl_->id_seed_;
    FiberContextImpl*	pScript = new FiberContextImpl();
    pScript->id_		= script_id;
    pScript->script_	= script;
    pScript->callbacker_= callbacker;
    pScript->param_		= param;
    pScript->stack_size_= stack_size;

    bool	bSucceed	= wait && !is_in_msg_thread()
                          ?	(0 != send_msg(Impl::OPERATION_EXECUTE_LOGIC, 0, LPARAM(pScript)))
                          :	(0 != post_msg(Impl::OPERATION_EXECUTE_LOGIC, 0, LPARAM(pScript)))
                          ;
    if(!bSucceed) {
        delete pScript;
        script_id	= INVALID_FIBER_SCRIPT_ID;
    }

    return script_id;
}

bool Fiber_Executor::stop_script(int script_id) {
    return (0 != post_msg(Impl::OPERATION_DELETE_LOGIC, 0, script_id));
}

bool Fiber_Executor::is_script_running(int script_id) {
    Impl::SCRIPT_LIST::iterator	it		= impl_->scripts_.begin();
    Impl::SCRIPT_LIST::iterator	it_end	= impl_->scripts_.end();
    for(; it != it_end; ++it) {
        FiberContextImpl*	pScript = static_cast<FiberContextImpl*>(*it);
        if(pScript->id_ == script_id) {
            return pScript->is_running();
        }
    }

    return false;
}

size_t Fiber_Executor::get_scripts_sum() {
    return	impl_->scripts_.size();
}

void Fiber_Executor::clear_scripts() {
    send_msg(Impl::OPERATION_CLEAR_ALL, 0, 0);
}

bool Fiber_Executor::update(double dt, bool wait) {
    impl_->timestamp_	+= dt;
    bool ret = (0 != send_msg(Impl::OPERATION_UPDATE_LOGIC, 0, 0));

    while(!impl_->end_handlers_.empty()) {
        Impl::FiberScriptEndHandler	AHandler	= impl_->end_handlers_.front();
        impl_->end_handlers_.pop_front();

        if(AHandler.handler) {
            AHandler.handler(AHandler.id, AHandler.param);
        }
    }

    return ret;
}

void Fiber_Executor::do_before_run() {
    impl_->main_fiber_	= ::ConvertThreadToFiber(NULL);
}

void Fiber_Executor::do_after_run() {
    ::ConvertFiberToThread();		// windows 2000 not compatible.

    Impl::SCRIPT_LIST::iterator	it		= impl_->scripts_.begin();
    Impl::SCRIPT_LIST::iterator	it_end	= impl_->scripts_.end();
    for(; it != it_end; ++it) {
        FiberContextImpl*	pScript = static_cast<FiberContextImpl*>(*it);
        DeleteFiber(pScript->fiber_);
        delete	pScript;
    }

    impl_->scripts_.clear();
}
