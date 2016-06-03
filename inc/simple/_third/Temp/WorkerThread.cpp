#include <windows.h>
#include <process.h>
#include "WorkerThread.h"

//	Destructor- check to make sure we're not still running.
WorkerThread::~WorkerThread()
{
	Stop();
}

//	Start the work.
bool WorkerThread::Start()
{
	//	Check for the trivial and error case.
	if(m_status == Working)
	{
		return true;
	}

	if(m_status == Stopping)
	{
		return false;
	}

	//	The current status is 'Stopped' so we can start the collection thread.
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, (ThreadProcess), 
		this, NULL, (unsigned int*)&m_dwThreadID);
	if(m_hThread == NULL)
	{
		return false;
	}

	//	The thread has been created- we must wait until the worker class
	//	has been told explicitly it has started until we return.
	while(m_status != Working)
	{
		Sleep(1);
	}

	//	The status is now working, the thread is running- we're done.
	return true;
}

//	Stop the work.
bool WorkerThread::Stop()
{
	//	Check for the trivial case.
	if(m_status == Stopped)
		return true;

	//	Set the status to stopping.
	m_status = Stopping;

	//	Wait for the work to stop.
	while(m_status != Stopped)
		Sleep(1);

	//	We may as well zero the thread id- we can't use it now.
	m_dwThreadID = 0;
	m_hThread = NULL;

	//	Finally the work has stopped- so we can return.
	return true;
}

//	Terminate the thread.
bool WorkerThread::Terminate()
{
	//	Check for the trivial case.
	if(m_status == Stopped)
		return true;

	//	Set the status to stopping.
	m_status = Stopping;

	//	Kill the thread.
	::TerminateThread(m_hThread, 0);

	//	Zero the thread and return.
	m_status = Stopped;
	m_dwThreadID = 0;
	m_hThread = NULL;

	return true;
}

//	Pause the thread.
bool WorkerThread::Pause()
{
	//	Check for the trivial case.
	if(m_status == Stopped)
		return true;

	//	If we are currently not paused...
	if(m_status == Working)
	{
		//	...then pause the thread.
		::SuspendThread(m_hThread);
		m_status = Paused;
	}
	else if(m_status == Paused)
	{
		//	Or if we're paused, resume the thread.
		::ResumeThread(m_hThread);
		m_status = Working;
	}

	//	Et voila.
	return true;
}

//	The main thread process to start collection.
unsigned int WorkerThread::ThreadProcess(void* lpParameter)
{
	//	Cast the parameter.
	WorkerThread* pThis = reinterpret_cast<WorkerThread*>(lpParameter);

	//	Set the 'Working' flag.
	pThis->m_status = Working;

	//	Start the 'DoWork' function. The clients implement this.
	pThis->DoWork();

	//	If this function stops, then we're done working- we're stopped.
	pThis->m_status = Stopped;

	//	We're done, return success.
	return 0;
}