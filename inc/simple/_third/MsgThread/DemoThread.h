#pragma once

#include "MsgThread.h"

enum
{
	kMsgGoGetPizza,
	kMsgPizzaInTheHouse,
};

class DemoThread : public MsgThread
{
public:

	DemoThread();
	virtual	~DemoThread();

	DECLARE_MSG_MAP();

private:

	void	OnMsgGoGetPizza(
				WPARAM wParam,
				LPARAM lParam);
	void	OnMsgPizzaInTheHouse(
				WPARAM wParam,
				LPARAM lParam);
};
