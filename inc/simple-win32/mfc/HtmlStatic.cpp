#include	"stdafx.h"

#include	"simple-win32/drawhtml.h"
#include	"HtmlStatic.h"

void HtmlStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
    RECT	rc;
    GetClientRect(&rc);

    int nOldBkMode	= ::GetBkMode(lpDrawItemStruct->hDC);
    ::SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);
    if(m_sHtmlText.IsEmpty()) {
        CString	sWndText;
        this->GetWindowText(sWndText);
        ::DrawHTML(
            lpDrawItemStruct->hDC,
            sWndText.GetString(),
            sWndText.GetLength(),
            &rc,
            m_dTextFormat
        );
    } else {
        ::DrawHTML(
            lpDrawItemStruct->hDC,
            m_sHtmlText.GetString(),
            m_sHtmlText.GetLength(),
            &rc,
            m_dTextFormat
        );
    }
    ::SetBkMode(lpDrawItemStruct->hDC, nOldBkMode);
}

void	HtmlStatic::SetHtmlText(const CString& sText, bool bRedraw) {
    m_sHtmlText = sText;
    if(bRedraw)DoRedraw();
}
const CString&	HtmlStatic::GetHtmlText() {
    return m_sHtmlText;
}

void	HtmlStatic::SetHtmlFormat(DWORD dFormat, bool bRedraw) {
    m_dTextFormat = dFormat;
    if(bRedraw)DoRedraw();
}
DWORD	HtmlStatic::GetHtmlFormat() {
    return m_dTextFormat;
}

void	HtmlStatic::DoRedraw() {
    CWnd*	pParent	= GetParent();
    if(NULL != pParent) {
        CRect	rcWnd;
        GetWindowRect(rcWnd);
        pParent->ScreenToClient(rcWnd);
        pParent->InvalidateRect(rcWnd);
    }
    this->Invalidate();
}
