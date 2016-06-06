#if	!defined(IO_WORKER_THREAD_H__089BB269_EC67_495d_885A_CAB04219E370__INCLUDED_)
#define	IO_WORKER_THREAD_H__089BB269_EC67_495d_885A_CAB04219E370__INCLUDED_

#include	<deque>
#include	"WorkerThread.h"

//
//	IO 工作数据包
//
struct	IOWorkerPackage
{
	int			cmd;
	DWORD		flag;
	size_t		size;
	void*		data;
};

//
//	IO 工作接口
//
class	IOWorker
{
public:
	// 返回 -1：错误，返回0：成功发送，返回1：发送过程中
	virtual	int						ReadPackage(IOWorkerPackage* pPkg)					= 0;	// 读取数据包
	virtual	int						WritePackage(IOWorkerPackage* pPkg)					= 0;	// 写入数据包

	virtual	IOWorkerPackage*		CreatePackage(int cmd, const void* data, size_t size, DWORD flag = 0);
	virtual	void					DestroyPackage(IOWorkerPackage* pPkg);
};

//
//	IO 工作线程
//
class	IOWorkerThread : public WorkerThread
{
public:
									IOWorkerThread(void);
	virtual							~IOWorkerThread();

public:
	bool							Start(IOWorker* pWorker);
	void							SetInputEvent(HANDLE evt)							{ m_InputEvent = evt; };

	IOWorker*						GetWorker()											{ return m_pWorker; }

	bool							IsWorking();							//	判断是否工作状态
	void							DestroyPackage(IOWorkerPackage* pPkg);	//	销毁数据包

public:
	virtual IOWorkerPackage*		PopInputPackage();
	virtual void					PushOutputPackage(IOWorkerPackage* pPkg);

protected:
	virtual void					DoWork();

protected:
	IOWorker*						m_pWorker;
	HANDLE							m_InputEvent;

	CRITICAL_SECTION				m_InputLocker;
	CRITICAL_SECTION				m_OutputLocker;

	std::deque<IOWorkerPackage*>	m_InputPackages;
	std::deque<IOWorkerPackage*>	m_OutputPackages;
};

#endif	//IO_WORKER_THREAD_H__089BB269_EC67_495d_885A_CAB04219E370__INCLUDED_