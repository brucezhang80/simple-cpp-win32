#include	"stdafx.h"
#include	"BitmapDC.h"

BitmapDC::BitmapDC() : m_pOldBmp(NULL), m_pBitmap(NULL), m_nBmpWidth(0), m_nBmpHeight(0) {
}

BitmapDC::~BitmapDC() {
    UnInitialize();
}

void    BitmapDC::Initialize(int nWidth, int nHeight, CDC* pDC, COLORREF clrBackground) {
    UnInitialize();

    CClientDC dcScreen(NULL);
    if (NULL == pDC) {
        pDC = &dcScreen;
    }

    // make compatible device context and select bitmap into it
    m_pBitmap = new CBitmap();
    m_pBitmap->CreateCompatibleBitmap(pDC, nWidth, nHeight);
    CreateCompatibleDC(pDC);

    m_pOldBmp = SelectObject(m_pBitmap);
    CDC::SetMapMode(pDC->GetMapMode());

    // paint background in bitmap
    FillSolidRect(0, 0, nWidth, nHeight, clrBackground);

    m_nBmpWidth     = nWidth;
    m_nBmpHeight    = nHeight;
}

void    BitmapDC::UnInitialize() {
    m_nBmpWidth     = 0;
    m_nBmpHeight    = 0;

    if(NULL != m_pBitmap) {
        SelectObject(m_pOldBmp);
        delete m_pBitmap;
        m_pBitmap	= NULL;
    }

    HDC hDC	= Detach();
    if(NULL != hDC) {
        ::DeleteDC(hDC);
    }
}

void	BitmapDC::SetUseBitmap(bool bSelect) {
    if(bSelect) {
        if(NULL != m_pBitmap)   SelectObject(m_pBitmap);
    } else {
        if(NULL != m_pOldBmp)   SelectObject(m_pOldBmp);
    }
}

GDIAutoPen::GDIAutoPen(CDC *pDC, int nPenStyle, int nWidth, COLORREF crColor) : CPen(nPenStyle, nWidth, crColor), m_pDC(pDC), m_pOldGdi(NULL) {
    Initialize();
}

GDIAutoPen::GDIAutoPen(CDC *pDC, int nPenStyle, int nWidth, const LOGBRUSH *pLogBrush,int nStyleCount, const DWORD *lpStyle)
    : CPen(nPenStyle, nWidth, pLogBrush, nStyleCount, lpStyle), m_pDC(pDC), m_pOldGdi(NULL) {
    Initialize();
}

GDIAutoPen::~GDIAutoPen() {
    ASSERT_VALID(m_pOldGdi);
    ASSERT_VALID(m_pDC);
    m_pDC->SelectObject(m_pOldGdi);
}

void GDIAutoPen::Initialize() {
    m_pOldGdi = m_pDC->SelectObject(this);
    ASSERT_VALID(m_pOldGdi);
}

GDIAutoBrush::GDIAutoBrush(CDC *pDC, COLORREF crColor) : CBrush(crColor), m_pDC(pDC), m_pOldGdi(NULL) {
    Initialize();
}

GDIAutoBrush::GDIAutoBrush(CDC *pDC, int nIndex, COLORREF crColor) : CBrush(nIndex, crColor), m_pDC(pDC), m_pOldGdi(NULL) {
    Initialize();
}

GDIAutoBrush::GDIAutoBrush(CDC *pDC, CBitmap* pBitmap) : CBrush(pBitmap), m_pDC(pDC), m_pOldGdi(NULL) {
    Initialize();
}

GDIAutoBrush::~GDIAutoBrush() {
    ASSERT_VALID(m_pOldGdi);
    ASSERT_VALID(m_pDC);
    m_pDC->SelectObject(m_pOldGdi);
}

void GDIAutoBrush::Initialize() {
    m_pOldGdi = m_pDC->SelectObject(this);
    ASSERT_VALID(m_pOldGdi);
}
