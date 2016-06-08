#include	<Windows.h>
#include	"bitmaphdc.h"

BitmapHDC::BitmapHDC() : m_hDC(NULL), m_hOldBmp(NULL), m_hBitmap(NULL), m_nBmpWidth(0), m_nBmpHeight(0) {
}

BitmapHDC::~BitmapHDC() {
    uninitialize();
}

void    BitmapHDC::initialize(int nWidth, int nHeight, HDC hDC, COLORREF clrBackground) {
    uninitialize();

    HDC dcScreen	= NULL;
    if (NULL == hDC) {
        hDC	= dcScreen	= ::GetDC(::GetDesktopWindow());
    }

    // make compatible device context and select bitmap into it
    m_hBitmap	= CreateCompatibleBitmap(hDC, nWidth, nHeight);
    m_hDC		= CreateCompatibleDC(hDC);

    m_hOldBmp	= (HBITMAP)::SelectObject(m_hDC, m_hBitmap);
    ::SetMapMode(m_hDC, ::GetMapMode(hDC));

    // paint background in bitmap
    {
        RECT	rcRect	= {0, 0, nWidth, nHeight};
        ::SetBkMode		(m_hDC, TRANSPARENT);
        ::SetBkColor	(m_hDC, clrBackground);
        ::ExtTextOut	(m_hDC, 0, 0, ETO_OPAQUE, &rcRect, NULL, 0, NULL);
    }

    m_nBmpWidth     = nWidth;
    m_nBmpHeight    = nHeight;

    if(NULL != dcScreen) {
        ::ReleaseDC(::GetDesktopWindow(), dcScreen);
    }
}

void    BitmapHDC::uninitialize() {
    m_nBmpWidth     = 0;
    m_nBmpHeight    = 0;

    if(NULL != m_hDC) {
        ::DeleteDC(m_hDC);
        m_hDC	= NULL;
    }

    if(NULL != m_hBitmap) {
        ::DeleteObject(m_hBitmap);
    }
}

void	BitmapHDC::set_use_bitmap(bool bSelect) {
    if(bSelect) {
        if(NULL != m_hBitmap)   ::SelectObject(m_hDC, m_hBitmap);
    } else {
        if(NULL != m_hOldBmp)   ::SelectObject(m_hDC, m_hOldBmp);
    }
}
