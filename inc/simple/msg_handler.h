#ifndef MSG_HANDLER_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define MSG_HANDLER_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	Windows 消息分发处理
//
class Msg_Handler {
public:
    typedef			void (Msg_Handler::*HANDLER)(WPARAM, LPARAM);

public:
    // 是否可以处理
    bool			has_handler(UINT msg) {
        return (0 != this->do_find_msg_handler(msg));
    }
    // 处理消息
    bool			handle_msg(UINT msg, WPARAM wParam,LPARAM lParam) {
        HANDLER	handler	= this->do_find_msg_handler(msg);
        if(NULL == handler) {
            return	false;
        }

        (this->*handler)(wParam, lParam);
        return true;
    }

protected:
    virtual	HANDLER	do_find_msg_handler(UINT msg) {
        return	0;
    }
};

#define MSG_HANDLER_BEGIN() 			virtual	HANDLER	do_find_msg_handler(UINT msg){\
											switch(msg){
#define MSG_HANDLER(nMsg, memberFn) 		case (nMsg):	return	Msg_Handler::HANDLER(memberFn);break;
#define MSG_HANDLER_END()					default:		return	__super::do_find_msg_handler(msg);\
											}\
										}

#endif
