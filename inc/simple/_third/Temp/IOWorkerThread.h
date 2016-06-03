#if	!defined(IO_WORKER_THREAD_H__089BB269_EC67_495d_885A_CAB04219E370__INCLUDED_)
#define	IO_WORKER_THREAD_H__089BB269_EC67_495d_885A_CAB04219E370__INCLUDED_

#include	<deque>
#include	"WorkerThread.h"

//
//	IO �������ݰ�
//
struct	IOWorkerPackage
{
	int			cmd;
	DWORD		flag;
	size_t		size;
	void*		data;
};

//
//	IO �����ӿ�
//
class	IOWorker
{
public:
	// ���� -1�����󣬷���0���ɹ����ͣ�����1�����͹�����
	virtual	int						ReadPackage(IOWorkerPackage* pPkg)					= 0;	// ��ȡ���ݰ�
	virtual	int						WritePackage(IOWorkerPackage* pPkg)					= 0;	// д�����ݰ�

	virtual	IOWorkerPackage*		CreatePackage(int cmd, const void* data, size_t size, DWORD flag = 0);
	virtual	void					DestroyPackage(IOWorkerPackage* pPkg);
};

//
//	IO �����߳�
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

	bool							IsWorking();							//	�ж��Ƿ���״̬
	void							DestroyPackage(IOWorkerPackage* pPkg);	//	�������ݰ�

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