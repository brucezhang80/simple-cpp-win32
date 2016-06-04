#include	<assert.h>

#define    WIN32_LEAN_AND_MEAN   //去除一些不常用的
#include	<windows.h>
#include	<limits.h>

#include	"NamedPipe.h"
#include	"NamedPipeIOThread.h"

//
//	数据包 定义
//
struct	NamedPipeIOPackage : public IOWorkerPackage
{
public:
	bool		head_readed;
	DWORD		byte_written;
};

//
//	IO 工作接口
//
class	NamedPipeIOWorker : public IOWorker
{
public:
	NamedPipeIOWorker(NamedPipe*	pPipe = NULL);

	void							SetNamedPipe(NamedPipe*	pPipe);
	NamedPipe*						GetNamedPipe();

public:
	// 返回 -1：错误，返回0：成功发送，返回1：发送过程中
	virtual	int						ReadPackage(IOWorkerPackage* pPkg);
	virtual	int						WritePackage(IOWorkerPackage* pPkg);

	virtual	IOWorkerPackage*		CreatePackage(int cmd, const void* data, size_t size, DWORD flag = 0);
	virtual	void					DestroyPackage(IOWorkerPackage* pPkg);

protected:
	NamedPipe*	m_pPipe;
};

NamedPipeIOWorker::NamedPipeIOWorker(NamedPipe*	pPipe)
:	m_pPipe(pPipe)
{
}

void NamedPipeIOWorker::SetNamedPipe(NamedPipe* pPipe)
{
	m_pPipe	= pPipe;
}

NamedPipe* NamedPipeIOWorker::GetNamedPipe()
{
	return	m_pPipe;
}

// 返回 -1：错误，返回0：成功发送，返回1：发送过程中
int NamedPipeIOWorker::ReadPackage(IOWorkerPackage* pPkg)
{
	assert(NULL != m_pPipe);
	if(NULL == pPkg || NULL == m_pPipe)
	{
		return	-1;
	}

	NamedPipeIOPackage*	pThePkg	= (NamedPipeIOPackage*)pPkg;

	// 读包头
	if(!pThePkg->head_readed)
	{
		DWORD	dwBytes	= 0;
		if(!::PeekNamedPipe(m_pPipe->GetHandle(), NULL, 0, NULL, &dwBytes, NULL))
		{
			return	-1;
		}
		if(sizeof(DWORD) * 3 > dwBytes)
		{
			return	1;
		}

		m_pPipe->Read(&(pThePkg->cmd), sizeof(pThePkg->cmd));
		m_pPipe->Read(&(pThePkg->flag), sizeof(pThePkg->flag));
		m_pPipe->Read(&(pThePkg->size), sizeof(pThePkg->size));

		delete	pThePkg->data;
		pThePkg->data	= new char[(pThePkg->size / 16 + 1) * 16];

		pThePkg->head_readed	= true;
	}

	// 读包体
	if(pThePkg->size > 0)
	{
		DWORD	dwBytes	= 0;
		if(!::PeekNamedPipe(m_pPipe->GetHandle(), NULL, 0, NULL, &dwBytes, NULL))
		{
			return	-1;
		}

		if(dwBytes < pThePkg->size)
		{
			return	1;
		}

		// 获取数据包体
		if(pThePkg->size != m_pPipe->Read(pThePkg->data, DWORD(pThePkg->size)))
		{
			return	-1;
		}
	}

	return	0;
}

int NamedPipeIOWorker::WritePackage(IOWorkerPackage* pPkg)
{
	assert(NULL != m_pPipe);
	if(NULL == pPkg || NULL == m_pPipe)
	{
		return	-1;
	}

	NamedPipeIOPackage*	pThePkg	= (NamedPipeIOPackage*)pPkg;

	DWORD	dwBuffer[3];
	dwBuffer[0]	= pThePkg->cmd;
	dwBuffer[1]	= pThePkg->flag;
	dwBuffer[2]	= DWORD(pThePkg->size);
	if(sizeof(dwBuffer) != m_pPipe->Write(dwBuffer, sizeof(dwBuffer)))
	{
		return -1;
	}

	if(pThePkg->size > 0)
	{
		int	nByteWritten	= m_pPipe->Write((char*)(pThePkg->data) + pThePkg->byte_written, DWORD(pThePkg->size) - pThePkg->byte_written);
		if(nByteWritten < 0)
		{
			return	-1;
		}

		pThePkg->byte_written	+= nByteWritten;
	}

	return	(pThePkg->byte_written == pThePkg->size)?0:1;
}

IOWorkerPackage* NamedPipeIOWorker::CreatePackage(int cmd, const void* data, size_t size, DWORD flag)
{
	NamedPipeIOPackage*	pPkg	= new NamedPipeIOPackage;

	pPkg->cmd	= cmd;
	pPkg->flag	= flag;

	pPkg->byte_written	= 0;
	pPkg->head_readed	= false;

	if(NULL != data && size > 0)
	{
		pPkg->data	= new char[(size/16 + 1) * 16];
		pPkg->size	= size;

		memcpy(pPkg->data, data, size);
	}
	else
	{
		pPkg->data	= NULL;
		pPkg->size	= 0;
	}

	return	pPkg;
}

void NamedPipeIOWorker::DestroyPackage(IOWorkerPackage* pPkg)
{
	if(NULL != pPkg)
	{
		delete	pPkg->data;
		delete	(NamedPipeIOPackage*)pPkg;
	}
}

NamedPipeClientIOThread::NamedPipeClientIOThread()
{
	m_pPipe		= new NamedPipeClient();
	m_pWorker	= new NamedPipeIOWorker(m_pPipe);
}

NamedPipeClientIOThread::~NamedPipeClientIOThread()
{
	delete	m_pWorker;
	delete	m_pPipe;
}

bool NamedPipeClientIOThread::Start(const char* pszPipeName)
{
	if(!m_pPipe->Open(pszPipeName))
	{
		return	false;
	}

	return	__super::Start(m_pWorker);
}

NamedPipeServerIOThread::NamedPipeServerIOThread()
:	m_bPipeConnected(false)
{
	m_pPipe		= new NamedPipeServer();
	m_pWorker	= new NamedPipeIOWorker(m_pPipe);
}

NamedPipeServerIOThread::~NamedPipeServerIOThread()
{
	delete	m_pWorker;
	delete	m_pPipe;
}

bool NamedPipeServerIOThread::Start(const char* pPipeName, DWORD dwInBuffer, DWORD dwOutBuffer, SECURITY_ATTRIBUTES* pSecAttrs, DWORD* pErrCode)
{
	if(!m_pPipe->Open(pPipeName, dwInBuffer, dwOutBuffer, pSecAttrs, pErrCode))
	{
		return	false;
	}

	return	__super::Start(m_pWorker);
}

void NamedPipeServerIOThread::DoWork()
{
	if(Working != GetStatus())
	{
		return;
	}

	m_bPipeConnected	= false;
	if(m_pPipe->Connect())
	{
		m_bPipeConnected	= true;

		{
			__super::DoWork();
		}

		m_pPipe->Disconnect();
		m_bPipeConnected	= false;
	}
}

bool NamedPipeServerIOThread::Stop()
{
	//	Check for the trivial case.
	if(m_status == Stopped)
	{
		return true;
	}

	//	Set the status to stopping.
	if(Working == this->GetStatus())
	{
		m_status = Stopping;
		if(!m_bPipeConnected)
		{
			NamedPipeClient	client;
			client.Open(m_pPipe->GetName().c_str());
			client.Close();
		}
	}

	return	__super::Stop();
}
