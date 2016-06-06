#include "stdafx.h"
#include "DemoThread.h"

DemoThread::DemoThread()
{
}

DemoThread::~DemoThread()
{
}

BEGIN_MSG_MAP(DemoThread, MsgThread)

	ON_MSG(kMsgGoGetPizza,			OnMsgGoGetPizza			)
	ON_MSG(kMsgPizzaInTheHouse,		OnMsgPizzaInTheHouse	)
	
END_MSG_MAP()

void
DemoThread::OnMsgGoGetPizza
(
	WPARAM wParam,
	LPARAM lParam
)
{
	int nDollar = (int) wParam;
	DemoThread* pReq = (DemoThread*) lParam;

	_tprintf(
		_T("tid=%d getting pizza for $%d\r\n"),
		m_nThreadId,
		nDollar);
	Sleep(5000);

	pReq->PostMessage(
		kMsgPizzaInTheHouse,
		wParam,
		0);
}

void
DemoThread::OnMsgPizzaInTheHouse
(
	WPARAM wParam,
	LPARAM lParam
)
{
	int nPizza = (int) wParam;

	_tprintf(
		_T("tid=%d got %d pizza. Thanks bro!\r\n"),
		m_nThreadId,
		nPizza);
}
