#pragma once

#include <deque>

#include "simple/delegate.h"

#include "simple-win32/dynamic.h"
#include "simple-win32/flashcall.h"

#include "FlashCtrl.h"

class	DynamicFunction;

class FlashCtrlEx : public FlashCtrl {
public:
    FlashCtrlEx();
    ~FlashCtrlEx();

protected:
    DECLARE_DYNCREATE(FlashCtrlEx)

public:
    FastDelegate<bool(FlashCtrlEx* pCtrl, long newState)>					evtReadyStateChange;
    FastDelegate<bool(FlashCtrlEx* pCtrl, long percentDone)>				evtLoadProgress;
    FastDelegate<bool(FlashCtrlEx* pCtrl, LPCTSTR command, LPCTSTR args)>	evtFSCommand;
    FastDelegate<bool(FlashCtrlEx* pCtrl, LPCTSTR request)>				evtFlashCall;

    void			SetFlashCallHandler(DynamicFunction* handler) {
        flash_call_handler_=handler;
    }
    DynamicFunction*GetFlashCallHandler() {
        return flash_call_handler_;
    }

public:
    bool			LoadMovie(void* data, size_t size);
    void			LoadMovie(long layer, const char* url);

    template<typename ValueType>
    bool		Call_GetReturnValue(ValueType& value) {
        DynamicData	data;
        if(		ret_.IsEmpty()
                ||	!FlashCallXML_DeSerializeData(ret_.GetString(), data)
          ) {
            return	false;
        }

        return	data.GetValue(value);
    }

    bool			Call(const char* pszFunction) {
        std::deque<DynamicData*>	args;
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename Arg1>
    bool			Call(const char* pszFunction, const Arg1& a1) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename Arg1, typename Arg2>
    bool			Call(const char* pszFunction, const Arg1& a1, const Arg2& a2) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename Arg1, typename Arg2, typename Arg3>
    bool			Call(const char* pszFunction, const Arg1& a1, const Arg2& a2, const Arg3& a3) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4>
    bool			Call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5>
    bool			Call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    bool			Call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    bool			Call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        PA(args, a7);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    bool			Call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        PA(args, a7);
        PA(args, a8);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    bool			Call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        PA(args, a7);
        PA(args, a8);
        PA(args, a9);
        return	Internal_CallFunction(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
    bool			Call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        PA(args, a7);
        PA(args, a8);
        PA(args, a9);
        PA(args, a10);
        return	Internal_CallFunction(pszFunction, args);
    }

private:
    void			Internal_OnReadyStateChange(long newState);
    void			Internal_OnProgress(long percentDone);
    void			Internal_OnFSCommand(LPCTSTR command, LPCTSTR args);
    void			Internal_OnFlashCall(LPCTSTR request);
    bool			Internal_CallFunction(const char* pszFunction, std::deque<DynamicData*>& args);

    template<typename ArgType>
    inline	void	Internal_PushAugument(std::deque<DynamicData*>& args, const ArgType& arg) {
        int	nIndex	= args.size();
        args_[nIndex]	= arg;
        args.push_back(&args_[nIndex]);
    }
    inline	void	Internal_PushAugument(std::deque<DynamicData*>& args, const DynamicData& arg) {
        args.push_back(const_cast<DynamicData*>(&arg));
    }

    template<typename ArgType>
    inline	void	PA(std::deque<DynamicData*>& args, const ArgType& arg) {
        Internal_PushAugument(args, arg);
    }

private:
    CString				ret_;
    DynamicData			args_[10];
    DynamicFunction*	flash_call_handler_;

protected:
    DECLARE_EVENTSINK_MAP()
};
