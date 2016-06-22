
////////////////////////////////////////////////////////////////////////////
//	Copyright : Amit Dey 2001
//
//	Email :amitdey@crosswinds.net
//
//	This code may be used in compiled form in any way you desire. This
//	file may be redistributed unmodified by any means PROVIDING it is 
//	not sold for profit without the authors written consent, and 
//	providing that this notice and the authors name is included.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
//	Description : ATL Superclassed Ownerdraw.Bitmap Button. 
////////////////////////////////////////////////////////////////////////////

// Dialog.h : Declaration of the CDialog

#ifndef __DIALOG_H_
#define __DIALOG_H_

#include "resource.h"       // main symbols
#include <atlhost.h>
#include "stdafx.h"
#include "AtlBitmapSkinButton.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CDialog
class CDialog : 
	public CDialogImpl < CDialog>
{
public:
CDialog()
{
}

~CDialog()
{
}
enum { IDD = IDD_DIALOG };
BEGIN_MSG_MAP(CDialog)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(ID_QUIT, OnQuit)
	REFLECT_NOTIFICATIONS()
	
	END_MSG_MAP()
	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	
	LRESULT OnQuit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(0);
	return 0;
}

LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();	
	SetIcon(LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1)));
	
	m_button1.SubclassWindow(GetDlgItem(IDC_BUTTON1));
	m_button1.LoadStateBitmaps(IDB_LOADU, IDB_LOADD, IDB_LOAD);
	
	m_button2.SubclassWindow(GetDlgItem(IDC_BUTTON2));
	m_button2.LoadStateBitmaps(IDB_PLAYU, IDB_PLAYD, IDB_PLAY);
	
	m_button3.SubclassWindow(GetDlgItem(IDC_BUTTON3));
	m_button3.LoadStateBitmaps(IDB_SAVEU, IDB_SAVED, IDB_SAVE);
	
	m_button4.SubclassWindow(GetDlgItem(ID_QUIT));
	m_button4.LoadStateBitmaps(IDB_QUITU, IDB_QUITD, IDB_QUIT);
	
	return 1;  // Let the system set the focus
}

LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}
private:
CAtlBitmapButton m_button1, m_button2, m_button3, m_button4;
};

#endif //__DIALOG_H_
