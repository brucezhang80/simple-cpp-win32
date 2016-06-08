#pragma once

#include <windows.h>

class CDlgTemplateBuilder {
public:
	CDlgTemplateBuilder(UINT nBufLen=1024);
	~CDlgTemplateBuilder();

public:
	// functions to build the template
	DLGTEMPLATE* Begin(DWORD dwStyle, const RECT& rc, LPCTSTR caption, DWORD dwStyleEx=0);
	DLGTEMPLATE* GetTemplate() { return (DLGTEMPLATE*)m_pBuffer; }

	// Windows predefined atom names
	enum { BUTTON=0x0080, EDIT, STATIC, LISTBOX, SCROLLBAR, COMBOBOX };

	void AddItem(WORD wType, DWORD dwStyle, const RECT& rc, LPCTSTR text, WORD nID=-1, DWORD dwStyleEx=0);
	void AddItem(WORD wType, DWORD dwStyle, const RECT& rc, WORD nResID, WORD nID=-1, DWORD dwStyleEx=0);

protected:
	WORD* AddText(WORD* buf, LPCTSTR text);
	// align ptr to nearest DWORD
	WORD* AlignDWORD(WORD* ptr) {
		ptr++;									 // round up to nearest DWORD
		LPARAM lp = (LPARAM)ptr;			 // convert to long
		lp &= 0xFFFFFFFC;						 // make sure on DWORD boundary
		return (WORD*)lp;
	}
	void AddItemTemplate(WORD wType, DWORD dwStyle, const RECT& rc, WORD nID, DWORD dwStyleEx);

protected:
	WORD* m_pBuffer;							 // internal buffer holds dialog template
	WORD* m_pNext;								 // next WORD to copy stuff
	WORD* m_pEndBuf;							 // end of buffer
};
