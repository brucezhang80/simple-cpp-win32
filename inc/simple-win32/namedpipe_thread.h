#ifndef NAMEDPIPE_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define NAMEDPIPE_THREAD_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<cassert>
#include	"simple/package.h"
#include	"io_thread.h"

class	NamedPipe;

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
//
//
class	NamedPipe_ServerReader : public IO_Reader{
public:
	NamedPipe_ServerReader();

protected:
    virtual	void	do_run();

private:
	volatile bool	m_client_connected;
};

//
//
//
class	NamedPipe_ServerWriter : public IO_Writer{
protected:
    virtual	void	do_run();
};

//
//
//
class	NamedPipe_ClientReader : public IO_Reader{
};

//
//
//
class	NamedPipe_ClientWriter : public IO_Writer{
};

#endif
