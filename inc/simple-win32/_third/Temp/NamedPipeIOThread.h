#pragma	once

#include "IOWorkerThread.h"

class	NamedPipeClient;
class	NamedPipeServer;

//
//	命名管道线程
//
class	NamedPipeClientIOThread	: public IOWorkerThread
{
public:
	NamedPipeClientIOThread();
	~NamedPipeClientIOThread();

public:
	bool				Start(const char* pszPipeName);

protected:
	NamedPipeClient*	m_pPipe;
};

class	NamedPipeServerIOThread	: public IOWorkerThread
{
public:
	NamedPipeServerIOThread();
	~NamedPipeServerIOThread();

public:
	bool				Start(const char* pPipeName, DWORD dwInBuffer = 0, DWORD dwOutBuffer = 0, SECURITY_ATTRIBUTES* pSecAttrs = NULL, DWORD* pErrCode = NULL);
	virtual bool		Stop();

protected:
	virtual	void		DoWork();

protected:
	NamedPipeServer*	m_pPipe;
	volatile bool		m_bPipeConnected;
};
