#include	<Windows.h>
#include	"namedpipe.h"

void	NamedPipe::do_close() {
    if(INVALID_HANDLE_VALUE != pipe_) {
        ::CloseHandle(pipe_);
        pipe_	= INVALID_HANDLE_VALUE;
    }
}

size_t	NamedPipe::do_read(void* pBuffer, size_t size) {
    DWORD	dwSize	= 0;
    if(!::ReadFile(pipe_, pBuffer, DWORD(size), &dwSize, NULL)) {
        return	-1;
    }

    return	size_t(dwSize);
}

size_t	NamedPipe::do_write(void* pBuffer, size_t size) {
    DWORD	dwSize	= 0;
    if(!::WriteFile(pipe_, pBuffer, DWORD(size), &dwSize, NULL)) {
        return	-1;
    }

    return	size_t(dwSize);
}

bool	NamedPipe_Server::do_open(const char* pName, DWORD dwInBuffer, DWORD dwOutBuffer, SECURITY_ATTRIBUTES* pSecAttrs, DWORD* pErrCode) {
    this->close();
    pipe_			= ::CreateNamedPipe(pName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE|PIPE_WAIT,
                                        PIPE_UNLIMITED_INSTANCES, dwOutBuffer, dwInBuffer, 300, pSecAttrs);

    if(NULL != pErrCode) {
        *pErrCode	= ::GetLastError();
    }

    return			(INVALID_HANDLE_VALUE != pipe_);
}

bool	NamedPipe_Server::do_connect() {
    if(NULL != pipe_) {
        return		(0 != ::ConnectNamedPipe(pipe_, NULL));
    }

    return	true;
}

bool	NamedPipe_Server::do_disconnect() {
    if(NULL != pipe_) {
        ::FlushFileBuffers(pipe_);

        return		(0 != ::DisconnectNamedPipe(pipe_));
    }

    return	true;
}

bool	NamedPipe_Client::do_open(const char* pName, DWORD timeout = 5000, SECURITY_ATTRIBUTES* pSecAttrs = NULL) {
    this->close();

    pipe_ = INVALID_HANDLE_VALUE;
    if(::WaitNamedPipe(pName, timeout)) {
        pipe_	= ::CreateFile(pName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, pSecAttrs,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    return		(INVALID_HANDLE_VALUE != pipe_);
}
