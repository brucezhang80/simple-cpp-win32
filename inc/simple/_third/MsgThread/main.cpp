#include "stdafx.h"
#include "DemoThread.h"

int
_tmain
(
	int		argc,
	TCHAR*	argv[]
)
{
	DemoThread rThread1, rThread2;

	rThread1.Start();
	rThread2.Start();

	rThread1.PostMessage(
		kMsgGoGetPizza,
		(WPARAM) 5,
		(LPARAM) &rThread2);

	rThread1.Stop();
	rThread2.Stop();

	return 0;
}
