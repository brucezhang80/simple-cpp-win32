#pragma once

#include <windows.h>

class MsgHandler;
typedef void (MsgHandler::*HMSG)(WPARAM, LPARAM);

struct MsgEntry
{
	UINT nMsg;
	HMSG pFn;
};

struct MsgMap
{
	const MsgMap*	pBaseMap;
	const MsgEntry* lpEntries;
};

#define DECLARE_MSG_MAP() \
private: \
	static const MsgEntry m_MsgEntryList[]; \
protected: \
	static const MsgMap m_MsgMap; \
	virtual const MsgMap* GetMsgMap() const; \

#define BEGIN_MSG_MAP(theClass, baseClass) \
	const MsgMap* theClass::GetMsgMap() const \
		{ return &theClass::m_MsgMap; } \
	const MsgMap theClass::m_MsgMap = \
	{ &baseClass::m_MsgMap, &theClass::m_MsgEntryList[0] }; \
	const MsgEntry theClass::m_MsgEntryList[] = \
	{ \

#define END_MSG_MAP() \
		{0, (HMSG) 0 } \
	}; \

#define ON_MSG(nMsg, memberFn) \
	{ nMsg, (HMSG) &memberFn },

class MsgHandler
{
public:

	MsgHandler();
	virtual	~MsgHandler();

	BOOL				IsHandled(const UINT msg);
	BOOL				OnMsg(
							const UINT		msg,
							const WPARAM	wParam,
							const LPARAM	lParam);

	DECLARE_MSG_MAP();

private:

	const MsgEntry*		FindMsgEntry(
							const UINT		nMsg,
							const MsgEntry* lpEntry);
};

union MsgMapFunctions
{
	HMSG pFn;
	void (MsgHandler::*pFn_MSG)(WPARAM, LPARAM);
};