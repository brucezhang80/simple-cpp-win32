#include "MsgHandler.h"
#include <assert.h>

const MsgMap MsgHandler::m_MsgMap =
{
	NULL,
	&MsgHandler::m_MsgEntryList[0]
};

const MsgEntry MsgHandler::m_MsgEntryList[] =
{
	{ 0, (HMSG) 0 }
};

MsgHandler::MsgHandler()
{
}

MsgHandler::~MsgHandler()
{
}

const MsgMap*
MsgHandler::GetMsgMap()
const
{
	return &MsgHandler::m_MsgMap;
}

const MsgEntry*
MsgHandler::FindMsgEntry
(
	const UINT		nMsg,
	const MsgEntry* lpEntry
)
{
	while (lpEntry->pFn)
	{
		if (nMsg == lpEntry->nMsg)
		{
			return lpEntry;
		}
		lpEntry++;
	}
	return NULL;
}

BOOL
MsgHandler::OnMsg
(
	const UINT		msg,
	const WPARAM	wParam,
	const LPARAM	lParam
)
{
	const MsgMap* pMsgMap = GetMsgMap();
	const MsgEntry* pMsgEntry = NULL;

	for (; pMsgMap != NULL; pMsgMap = pMsgMap->pBaseMap)
	{
		assert( pMsgMap != pMsgMap->pBaseMap );

		pMsgEntry = FindMsgEntry(msg, pMsgMap->lpEntries);
		if (pMsgEntry)
		{
			break;
		}
	}
	
	if (!pMsgEntry)
	{
		return FALSE;
	}

	union MsgMapFunctions mmf;
	mmf.pFn = pMsgEntry->pFn;

	(this->*mmf.pFn_MSG)(wParam, lParam);

	return TRUE;
}

BOOL
MsgHandler::IsHandled
(
	const UINT	msg
)
{
	const MsgMap* pMsgMap = GetMsgMap();
	const MsgEntry* pMsgEntry = NULL;

	for (; pMsgMap != NULL; pMsgMap = pMsgMap->pBaseMap)
	{
		assert( pMsgMap != pMsgMap->pBaseMap );

		pMsgEntry = FindMsgEntry(msg, pMsgMap->lpEntries);
		if (pMsgEntry)
		{
			break;
		}
	}
	
	return (NULL != pMsgEntry);
}