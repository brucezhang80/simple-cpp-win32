#ifndef SE_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define SE_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	NEED LIBRARY:	Dbghelp.lib
//

#include	<string>
#include	<exception>
#include	<eh.h>
#include	<dbghelp.h>

// The following function retrieves exception info
EXCEPTION_POINTERS*				se_get_exception_pointers(DWORD dwExceptionCode);
void							se_destroy_exception_pointers(EXCEPTION_POINTERS* ppExceptionPointers);

//
//	结构化异常基类
//
struct se_exception : public std::exception {
public:
    se_exception(unsigned int code, _EXCEPTION_POINTERS* pep);

public:
    void						generate_dump()const;

    unsigned int				se_code()const {
        return code_;
    }
    _EXCEPTION_POINTERS*		se_pointers()const {
        return pep_;
    }
    void						se_module(char* pszModuleName, size_t size)const;

public:
    virtual const char *		what()const {
        return msg_.c_str();
    }

public:
    static	void				on_throw_exception(const se_exception& ex)							{}
    static	void				on_unhandled_exception(const se_exception& ex)						{}
    static	void				on_new_exception() {
        throw std::bad_alloc("no more memory!");
    }

protected:
    virtual	bool				on_get_dump_file_name(char* pszFileName, size_t size)const;
    virtual	DWORD				on_get_dump_data_falgs()const;
    virtual	bool				on_generate_dump(HANDLE hFile)const;

private:
    std::string					msg_;
    unsigned int				code_;
    _EXCEPTION_POINTERS*		pep_;
};

//
//	结构化异常安装类
//
template<typename SEException = se_exception>
struct se_handler {
public:
    // 为当前线程安装结构化异常到C++异常的转换器
    static	void				install_se_thread_translator() {
        _set_se_translator( handle_translate_exception );
    }
    // 为当前进程安装结构化异常处理器
    static	void				install_se_unhandled_handler() {
        SetUnhandledExceptionFilter( handle_se_unhandled );
    }
    // 为当前进程安装结构化异常处理器
    static	void				install_new_exception_handler() {
        std::set_new_handler( handle_new_exception );
    }
    // 生成Dump文件
    static	void				generate_dump(_EXCEPTION_POINTERS* ExceptionInfo = NULL) {
        handle_generate_dump(ExceptionInfo);
    }

private:
    static void __cdecl			handle_translate_exception(unsigned int code, _EXCEPTION_POINTERS* ExceptionInfo) {
        SEException	ex(code, ExceptionInfo);
        SEException::on_throw_exception(ex);
        throw	ex;
    }

    static long	WINAPI			handle_se_unhandled( _EXCEPTION_POINTERS *ExceptionInfo ) {
        SEException	ex(ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo);
        SEException::on_unhandled_exception(ex);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    static void __cdecl			handle_new_exception() {
        SEException::on_new_exception();
    }

    static void __cdecl			handle_generate_dump(_EXCEPTION_POINTERS* ExceptionInfo) {
        _EXCEPTION_POINTERS* pTheExceptionInfo	= ExceptionInfo;
        if(NULL == ExceptionInfo) {
            pTheExceptionInfo	= se_get_exception_pointers(5);
        }

        SEException	ex(pTheExceptionInfo->ExceptionRecord->ExceptionCode, pTheExceptionInfo);
        ex.generate_dump();

        if(NULL == ExceptionInfo) {
            se_destroy_exception_pointers(pTheExceptionInfo);
        }
    }

private:
    se_handler();
};

#endif
