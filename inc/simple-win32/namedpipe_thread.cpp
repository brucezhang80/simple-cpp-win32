#include	<Windows.h>
#include	"namedpipe_thread.h"
#include	"namedpipe.h"

//
//	数据包 定义
//
struct	NamedPipe_Package : public package {
public:
    bool		head_readed;
    DWORD		byte_written;
};

NamedPipe_PackageHandler::NamedPipe_PackageHandler(NamedPipe* pipe)
    :	m_pipe(pipe) {
}

void NamedPipe_PackageHandler::set_namedpipe(NamedPipe* pipe) {
    m_pipe	= pipe;
}

NamedPipe* NamedPipe_PackageHandler::get_namedpipe() {
    return	m_pipe;
}

// 返回 -1：错误，返回0：成功发送，返回1：发送过程中
int NamedPipe_PackageHandler::read_package(package* pkg) {
    assert(NULL != m_pipe);
    if(NULL == pkg || NULL == m_pipe) {
        return	-1;
    }

    NamedPipe_Package*	pThePkg	= (NamedPipe_Package*)pkg;

    // 读包头
    if(!pThePkg->head_readed) {
        DWORD	dwBytes	= 0;
        if(!::PeekNamedPipe(m_pipe->handle(), NULL, 0, NULL, &dwBytes, NULL)) {
            return	-1;
        }
        if(sizeof(DWORD) * 3 > dwBytes) {
            return	1;
        }

        m_pipe->read(&(pThePkg->cmd), sizeof(pThePkg->cmd));
        m_pipe->read(&(pThePkg->flag), sizeof(pThePkg->flag));
        m_pipe->read(&(pThePkg->size), sizeof(pThePkg->size));

        delete	pThePkg->data;
        pThePkg->data	= new char[(pThePkg->size / 16 + 1) * 16];

        pThePkg->head_readed	= true;
    }

    // 读包体
    if(pThePkg->size > 0) {
        DWORD	dwBytes	= 0;
        if(!::PeekNamedPipe(m_pipe->handle(), NULL, 0, NULL, &dwBytes, NULL)) {
            return	-1;
        }

        if(dwBytes < pThePkg->size) {
            return	1;
        }

        // 获取数据包体
        if(pThePkg->size != m_pipe->read(pThePkg->data, DWORD(pThePkg->size))) {
            return	-1;
        }
    }

    return	0;
}

int NamedPipe_PackageHandler::write_package(package* pkg) {
    assert(NULL != m_pipe);
    if(NULL == pkg || NULL == m_pipe) {
        return	-1;
    }

    NamedPipe_Package*	pThePkg	= (NamedPipe_Package*)pkg;

    DWORD	dwBuffer[3];
    dwBuffer[0]	= pThePkg->cmd;
    dwBuffer[1]	= pThePkg->flag;
    dwBuffer[2]	= DWORD(pThePkg->size);
    if(sizeof(dwBuffer) != m_pipe->write(dwBuffer, sizeof(dwBuffer))) {
        return -1;
    }

    if(pThePkg->size > 0) {
        int	nByteWritten	= m_pipe->write((char*)(pThePkg->data) + pThePkg->byte_written, DWORD(pThePkg->size) - pThePkg->byte_written);
        if(nByteWritten < 0) {
            return	-1;
        }

        pThePkg->byte_written	+= nByteWritten;
    }

    return	(pThePkg->byte_written == pThePkg->size)?0:1;
}

package* NamedPipe_PackageHandler::create_package(int32_t cmd, const void* data, size_t size, uint32_t flag) {
    NamedPipe_Package*	pkg	= new NamedPipe_Package;

    pkg->cmd	= cmd;
    pkg->flag	= flag;

    pkg->byte_written	= 0;
    pkg->head_readed	= false;

    if(NULL != data && size > 0) {
        pkg->data	= new char[(size/16 + 1) * 16];
        pkg->size	= size;

        memcpy(pkg->data, data, size);
    } else {
        pkg->data	= NULL;
        pkg->size	= 0;
    }

    return	pkg;
}

void NamedPipe_PackageHandler::destroy_package(package* pkg) {
    if(NULL != pkg) {
        delete	pkg->data;
        delete	(NamedPipe_Package*)pkg;
    }
}

//	server reader impl.
class	NamedPipe_ServerReader : public IO_ThreadReader {
public:
    NamedPipe_ServerReader(NamedPipe_Server* server)
        :	m_client_connected(false)
        ,	m_server(server) {
    }

protected:
    virtual	void	do_run() {
        m_client_connected	= false;
        if(m_server->connect()) {
            m_client_connected	= true;

            {
                __super::do_run();
            }

            m_server->disconnect();
            m_client_connected	= false;
        }
    }

    virtual	bool	do_stop(bool bKillOnTimeout, UINT uTimeout) {
        if(is_running() && !m_client_connected) {
            NamedPipe_Client	client;
            client.open(m_server->pipename());
            client.close();
        }

        return	IO_ThreadReader::do_stop(bKillOnTimeout, uTimeout);
    }

private:
    NamedPipe_Server*	m_server;
    volatile bool		m_client_connected;
};

//	server writer impl.
class	NamedPipe_ServerWriter : public IO_ThreadWriter {
};

//	client reader impl.
class	NamedPipe_ClientReader : public IO_ThreadReader {
};

//	client writer impl.
class	NamedPipe_ClientWriter : public IO_ThreadWriter {
};

NamedPipe_ThreadServer::NamedPipe_ThreadServer() {
    m_pkg_handler.set_namedpipe(&m_pipe);
    this->set_reader(new NamedPipe_ServerReader(&m_pipe));
    this->set_writer(new NamedPipe_ServerWriter());
    this->set_package_handler(&m_pkg_handler);
}

bool	NamedPipe_ThreadServer::start(const char* pipename, DWORD dwInBuffer, DWORD dwOutBuffer, SECURITY_ATTRIBUTES* pSecAttrs, DWORD* pErrCode) {
    if(!m_pipe.open(pipename, dwInBuffer, dwOutBuffer, pSecAttrs, pErrCode)) {
        return	false;
    }

    return	IO_ThreadWorker::start();
}

bool	NamedPipe_ThreadServer::do_start() {
    if(NULL == m_pipe.handle()) {
        return	false;
    }

    return	IO_ThreadWorker::do_start();
}

bool	NamedPipe_ThreadServer::do_stop(bool bKillOnTimeout, UINT uTimeout) {
    bool	ret	= IO_ThreadWorker::do_stop(bKillOnTimeout, uTimeout);
    m_pipe.close();
    return	ret;
}

NamedPipe_ThreadServer::~NamedPipe_ThreadServer() {
}

NamedPipe_ThreadClient::NamedPipe_ThreadClient() {
    m_pkg_handler.set_namedpipe(&m_pipe);
    this->set_reader(new NamedPipe_ClientReader());
    this->set_writer(new NamedPipe_ClientWriter());
    this->set_package_handler(&m_pkg_handler);
}

bool	NamedPipe_ThreadClient::start(const char* pipename) {
    if(!m_pipe.open(pipename)) {
        return	false;
    }

    return	IO_ThreadWorker::start();
}

bool	NamedPipe_ThreadClient::do_start() {
    if(NULL == m_pipe.handle()) {
        return	false;
    }

    return	IO_ThreadWorker::do_start();
}

bool	NamedPipe_ThreadClient::do_stop(bool bKillOnTimeout, UINT uTimeout) {
    bool	ret	= IO_ThreadWorker::do_stop(bKillOnTimeout, uTimeout);
    m_pipe.close();
    return	ret;
}

NamedPipe_ThreadClient::~NamedPipe_ThreadClient() {
}
