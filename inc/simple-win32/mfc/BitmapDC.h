#pragma	once

//
//  内存Bitmap绘图类
//
class CBitmap;
class BitmapDC : public CDC {
public:
    BitmapDC();
    virtual ~BitmapDC();

    void    Initialize(int nWidth, int nHeight, CDC* pDC = NULL, COLORREF clrBackground = RGB(255,255,255));
    void    UnInitialize();

    void	SetUseBitmap(bool bSelect);

    CBitmap*    GetBitmap() {
        return  m_pBitmap;
    }

    HBITMAP     GetHBITMAP() {
        return  (NULL == m_pBitmap)?NULL:m_pBitmap->operator HBITMAP();
    }

    int         GetBitmapWidth() {
        return  m_nBmpWidth;
    }

    int         GetBitmapHeight() {
        return  m_nBmpWidth;
    }

    // Allow usage as a pointer
    BitmapDC*  operator->() {
        return this;
    }

    // Allow usage as a pointer
    operator    BitmapDC*() {
        return this;
    }

protected:
    CBitmap*    m_pBitmap;
    CBitmap*    m_pOldBmp;
    int         m_nBmpWidth;
    int         m_nBmpHeight;
};

//
//	自动选择画笔
//
class GDIAutoPen : public CPen {
public:
    GDIAutoPen(CDC *pDC, int nPenStyle, int nWidth, COLORREF crColor);
    GDIAutoPen(CDC *pDC, int nPenStyle, int nWidth, const LOGBRUSH *pLogBrush,int nStyleCount = 0, const DWORD *lpStyle = NULL);
    virtual ~GDIAutoPen();

private:
    GDIAutoPen();
    void Initialize();

private:
    CDC			*m_pDC;      // @cmember remembers device context
    CGdiObject	*m_pOldGdi;  // @cmember remembers previous pen
};

//
//	自动选择画刷
//
class GDIAutoBrush : public CBrush {
public:
    GDIAutoBrush(CDC *pDC, COLORREF crColor);
    GDIAutoBrush(CDC *pDC, int nIndex, COLORREF crColor);
    GDIAutoBrush(CDC *pDC, CBitmap* pBitmap);
    virtual ~GDIAutoBrush();

private:
    GDIAutoBrush();
    void Initialize();

private:
    CDC        *m_pDC;      // @cmember remembers device context
    CGdiObject *m_pOldGdi;  // @cmember remembers previous pen
};
