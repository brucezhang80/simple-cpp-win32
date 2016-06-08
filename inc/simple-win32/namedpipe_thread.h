#ifndef NAMEDPIPE_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define NAMEDPIPE_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<cassert>
#include	"simple/package.h"
#include	"io_thread.h"
#include	"namedpipe.h"

//
//	NamedPipe package handler
//
class	NamedPipe_PackageHandler : public package_handler {
public:
    NamedPipe_PackageHandler(NamedPipe*	pipe = NULL);

    void				set_namedpipe(NamedPipe*	pipe);
    NamedPipe*			get_namedpipe();

public:
    // 返回 -1：错误，返回0：成功发送，返回1：发送过程中
    virtual	int			read_package(package* pkg);
    virtual	int			write_package(package* pkg);

    virtual	package*	create_package(int32_t cmd, const void* data, size_t size, uint32_t flag = 0);
    virtual	void		destroy_package(package* pkg);

protected:
    NamedPipe*			m_pipe;
};

//
//	Thread NamedPipe Server
//
class	NamedPipe_ThreadServer : public IO_ThreadWorker {
public:
    NamedPipe_ThreadServer();
    virtual			~NamedPipe_ThreadServer();

public:
    bool			start(const char* pipename, DWORD dwInBuffer = 0, DWORD dwOutBuffer = 0, SECURITY_ATTRIBUTES* pSecAttrs = NULL, DWORD* pErrCode = NULL);

protected:
    virtual	bool	do_start();
    virtual	bool	do_stop(bool bKillOnTimeout, UINT uTimeout);

private:
    NamedPipe_Server			m_pipe;
    NamedPipe_PackageHandler	m_pkg_handler;
};

//
//	Thread NamedPipe Client
//
class	NamedPipe_ThreadClient : public IO_ThreadWorker {
public:
    NamedPipe_ThreadClient();
    virtual			~NamedPipe_ThreadClient();

public:
    bool			start(const char* pipename);

protected:
    virtual	bool	do_start();
    virtual	bool	do_stop(bool bKillOnTimeout, UINT uTimeout);

private:
    NamedPipe_Client			m_pipe;
    NamedPipe_PackageHandler	m_pkg_handler;
};

#endif
