#include <atlbase.h>

#include "BitmapButton.h"

#include	"simple-win32/gdiplus/subimage.h"


BitmapButton::BitmapButton(void)
    :	NormalImage(NULL)
    ,	DownImage(NULL)
    ,	HoverImage(NULL)
    ,	DisabledImage(NULL)
    ,	m_tracking(false)
    ,	m_btndown(false) {
}


BitmapButton::~BitmapButton(void) {
}

UINT BitmapButton::state() const {
    ATLASSERT(::IsWindow(m_hWnd));
    return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0L);
}

void BitmapButton::set_state(bool selected) {
    ATLASSERT(::IsWindow(m_hWnd));
    ::SendMessage(m_hWnd, BM_SETSTATE, selected, 0L);
}

LRESULT BitmapButton::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
    return 1;
}


LRESULT BitmapButton::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
    if (m_tracking) {
        TRACKMOUSEEVENT t = {
            sizeof(TRACKMOUSEEVENT),
            TME_CANCEL | TME_LEAVE,
            m_hWnd,
            0
        };
        if (::_TrackMouseEvent(&t)) {
            m_tracking = false;
        }
    }

    m_btndown = true;

    bHandled = false;
    return 0;
}


LRESULT BitmapButton::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
    m_btndown = false;

    bHandled = false;
    return 0;
}


LRESULT BitmapButton::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
    if (::GetCapture() == m_hWnd) {
        ::ReleaseCapture();
        ATLASSERT(!m_tracking);
        m_btndown = false;
    }

    bHandled = false;
    return 0;
}


LRESULT BitmapButton::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled) {
    if ((m_btndown) &&(::GetCapture() == m_hWnd)) {
        POINT p2;
        p2.x = LOWORD(lParam);
        p2.y = HIWORD(lParam);

        ::ClientToScreen(m_hWnd, &p2);
        HWND mouse_wnd = ::WindowFromPoint(p2);

        bool pressed = ((state() & BST_PUSHED) == BST_PUSHED);
        bool need_pressed = (mouse_wnd == m_hWnd);
        if (pressed != need_pressed) {
            set_state(need_pressed ? TRUE : FALSE);
            Invalidate();
        }
    } else {
        if (!m_tracking) {
            TRACKMOUSEEVENT t = {
                sizeof(TRACKMOUSEEVENT),
                TME_LEAVE,
                m_hWnd,
                0
            };
            if (::_TrackMouseEvent(&t)) {
                m_tracking = true;
                Invalidate();
            }
        }
    }

    bHandled = false;
    return 0;
}


LRESULT BitmapButton::OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
    m_tracking = false;
    Invalidate();

    return 0;
}


LRESULT BitmapButton::OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) {
    POINT	mouse_pos;
    UINT	curr_state	= this->state();
    if ((m_btndown) &&(::GetCapture() == m_hWnd) &&(::GetCursorPos(&mouse_pos))) {
        if (::WindowFromPoint(mouse_pos) == m_hWnd) {
            if ((curr_state & BST_PUSHED) != BST_PUSHED) {
                set_state(true);
                return -1;
            }
        } else {
            if ((curr_state & BST_PUSHED) == BST_PUSHED) {
                set_state(false);
                return -1;
            }
        }
    }

    LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

    Gdiplus::Rect rect(
        lpdis->rcItem.left,
        lpdis->rcItem.top,
        lpdis->rcItem.right	- lpdis->rcItem.left,
        lpdis->rcItem.bottom- lpdis->rcItem.top
    );

    SubImage*	img	= this->NormalImage;
    if (lpdis->itemState & ODS_SELECTED) {
        img	= this->DownImage;
    } else if (lpdis->itemState & ODS_DISABLED) {
        img	= this->DisabledImage;
    } else if (m_tracking) {
        img	= this->HoverImage;
    }

    if(NULL != img && NULL != img->image) {
        Gdiplus::Graphics	graphics(lpdis->hDC);
        graphics.DrawImage(img->image, rect,
                           img->rect.X, img->rect.Y,
                           img->rect.Width, img->rect.Height,
                           Gdiplus::UnitPixel
                          );
    }

    return 0;
}
