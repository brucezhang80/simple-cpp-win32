
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
//	Description : Blah Blah Blah... 
////////////////////////////////////////////////////////////////////////////

// MouseTrack.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "dialog.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// TODO: Place code here.
	_Module.Init(0, hInstance);
	
	CDialog dlg;
	dlg.DoModal();
	
	_Module.Term();
	return 0;
}



