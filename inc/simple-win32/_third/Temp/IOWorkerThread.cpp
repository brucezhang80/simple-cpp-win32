
#if		!defined(WINVER)
#		define	WINVER	0x0501
#endif

#if		!defined(_WIN32_WINNT)
#		define	_WIN32_WINNT	0x0501
#endif

#include <cassert>

#include <Windows.h>
#include "IOWorkerThread.h"

class	LockGuard
{
public:
	LockGuard(CRITICAL_SECTION& locker) : m_Locker(locker)
	{
		EnterCriticalSection(&m_Locker);
	}
	~LockGuard()
	{
		LeaveCriticalSection(&m_Locker);
	}

private:
	CRITICAL_SECTION&	m_Locker;
};

IOWorkerPackage* IOWorker::CreatePackage(int cmd, const void* data, size_t size, DWORD flag)
{
	IOWorkerPackage*	pPkg	= new IOWorkerPackage;

	pPkg->cmd	= cmd;
	pPkg->flag	= flag;

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

void IOWorker::DestroyPackage(IOWorkerPackage* pPkg)
{
	if(NULL != pPkg)
	{
		delete	pPkg->data;
		delete	pPkg;
	}
}

IOWorkerThread::IOWorkerThread()
:	m_InputEvent(NULL)
,	m_pWorker(NULL)
{
	::InitializeCriticalSection(&m_InputLocker);
	::InitializeCriticalSection(&m_OutputLocker);
}

IOWorkerThread::~IOWorkerThread()
{
	::DeleteCriticalSection(&m_OutputLocker);
	::DeleteCriticalSection(&m_InputLocker);
}

bool IOWorkerThread::Start(IOWorker* pWorker)
{
	if(NULL == pWorker)
	{
		return	false;
	}

	//	Check for the trivial and error case.
	if(GetStatus() == Working)
	{
		return true;
	}
	if(GetStatus() == Stopping)
	{
		return false;
	}

	m_pWorker	= pWorker;
	return		__super::Start();
}

void IOWorkerThread::DoWork()
{
	if(NULL == m_pWorker)
	{
		return;
	}

	int					nWaitMS		= 0;
	IOWorkerPackage*	pInputPkg	= NULL;
	while(Working == GetStatus())
	{
		// 接收
		try
		{
			if(NULL == pInputPkg)
			{
				pInputPkg	= m_pWorker->CreatePackage(0, NULL, 0, 0);
			}

			int	nRet	= m_pWorker->ReadPackage(pInputPkg);

			if(0 > nRet)
			{
				break;
			}

			if(0 == nRet)
			{
				nWaitMS	= 0;

				{
					LockGuard	guard(m_InputLocker);
					m_InputPackages.push_back(pInputPkg);
					pInputPkg	= NULL;
				}

				if(NULL != m_InputEvent)
				{
					::SetEvent(m_InputEvent);
				}
			}
		}
		catch(...)
		{
			// log ???
		}

		// 发送
		try
		{
			IOWorkerPackage*	pOutputPkg	= NULL;
			{
				LockGuard	guard(m_OutputLocker);
				if(!m_OutputPackages.empty())
				{
					pOutputPkg	= m_OutputPackages.front();
				}
			}

			if(NULL != pOutputPkg)
			{
				int	nRet	= m_pWorker->WritePackage(pOutputPkg);

				if(0 > nRet)
				{
					break;
				}

				nWaitMS		= 0;
				if(0 == nRet)
				{
					LockGuard	guard(m_OutputLocker);
					m_OutputPackages.pop_front();
					m_pWorker->DestroyPackage(pOutputPkg);
				}
			}
		}
		catch(...)
		{
			// log ???
		}

		if(nWaitMS > 10)
		{
			nWaitMS	= 10;
		}

		::Sleep((nWaitMS / 5) * 5);

		nWaitMS	+= 5;
	}

	if(NULL != pInputPkg)
	{
		m_pWorker->DestroyPackage(pInputPkg);
	}
}

IOWorkerPackage* IOWorkerThread::PopInputPackage()
{
	LockGuard	guard(m_InputLocker);
	if(m_InputPackages.empty())
	{
		return	NULL;
	}
	else
	{
		IOWorkerPackage*	pPkg	= m_InputPackages.front();
		m_InputPackages.pop_front();

		return	pPkg;
	}
}

void IOWorkerThread::PushOutputPackage(IOWorkerPackage* pPkg)
{
	if(NULL != pPkg)
	{
		LockGuard	guard(m_OutputLocker);
		m_OutputPackages.push_back(pPkg);
	}
}

bool IOWorkerThread::IsWorking()
{
	return	(Working == GetStatus());
}

//	销毁数据包
void IOWorkerThread::DestroyPackage(IOWorkerPackage* pPkg)
{
	assert(NULL != m_pWorker);
	if(NULL != m_pWorker)
	{
		m_pWorker->DestroyPackage(pPkg);
	}
}
