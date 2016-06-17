#ifndef NAMEDPIPE_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define NAMEDPIPE_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<cassert>
#include	<string>

//
//	命名管道公共操作
//
class	NamedPipe {
public:
    NamedPipe() : pipe_(INVALID_HANDLE_VALUE)								{}
    virtual		~NamedPipe() {
        assert(pipe_ == INVALID_HANDLE_VALUE);
    }

public:
    HANDLE		handle() {
        return	pipe_;
    }

    void		close() {
        this->do_close();
    }
    size_t		read(void* pBuffer, size_t size) {
        return	this->do_read(pBuffer, size);
    }
    size_t		write(void* pBuffer, size_t size) {
        return	this->do_write(pBuffer, size);
    }

protected:
    virtual	void	do_close();
    virtual	size_t	do_read(void* pBuffer, size_t size);
    virtual	size_t	do_write(void* pBuffer, size_t size);

protected:
    HANDLE				pipe_;
};

//
//	命名管道服务端
//
class	NamedPipe_Server : public NamedPipe {
public:
    bool		open(const char* pName, DWORD dwInBuffer = 0, DWORD dwOutBuffer = 0, SECURITY_ATTRIBUTES* pSecAttrs = NULL, DWORD* pErrCode = NULL) {
        if(NULL == pName) {
            return	false;
        }
        m_pipename.assign(pName);
        return	this->do_open(pName, dwInBuffer, dwOutBuffer, pSecAttrs, pErrCode);
    }

    bool	    connect() {
        return	this->do_connect();
    }

    bool	    disconnect() {
        return	this->do_disconnect();
    }

    const char*	pipename() {
        return	m_pipename.c_str();
    }

protected:
    virtual	bool	do_open(const char* pName, DWORD dwInBuffer, DWORD dwOutBuffer, SECURITY_ATTRIBUTES* pSecAttrs, DWORD* pErrCode);
    virtual bool	do_connect();
    virtual bool	do_disconnect();

private:
    std::string		m_pipename;
};

//
//	命名管道客户端
//
class	NamedPipe_Client : public NamedPipe {
public:
    bool	open(const char* pName, DWORD timeout = 5000, SECURITY_ATTRIBUTES* pSecAttrs = NULL) {
        return	this->do_open(pName, timeout, pSecAttrs);
    }

protected:
    virtual	bool	do_open(const char* pName, DWORD timeout, SECURITY_ATTRIBUTES* pSecAttrs);
};

#endif
