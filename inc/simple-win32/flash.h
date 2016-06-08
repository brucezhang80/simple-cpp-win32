#ifndef FLASH_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define FLASH_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include <string>
#include <windows.h>

#include "simple/delegate.h"

struct IOleObject;
struct IOleInPlaceObjectWindowless;

namespace ShockwaveFlashObjects {
struct IShockwaveFlash;
}

class FlashSink;
class FlashControlSite;

class Flash_Player {
public:
    enum	FLASH_STATE {
        STATE_IDLE,
        STATE_PLAYING,
        STATE_STOPPED
    };

    enum	FLASH_QUALITY {
        QUALITY_LOW,
        QUALITY_MEDIUM,
        QUALITY_HIGH
    };

    typedef	delegate<void(Flash_Player*)>								EVENT_FLASH_STOPPED;
    typedef	delegate<void(Flash_Player*, const char*, const char*)>		EVENT_FLASH_COMMAND;

    typedef	delegate<HMODULE(Flash_Player*, void**)>					EVENT_LOAD_FLASH_LIBRARY;
    typedef	delegate<void(Flash_Player*, void*)>						EVENT_UNLOAD_FLASH_LIBRARY;

public:
    Flash_Player();
    ~Flash_Player();

    static double			get_flash_version();

    bool					play(const std::string& theFileName);
    bool					play(void* data, size_t length);

    void					set_quality(int theQuality);
    void					pause();
    void					resume();
    bool					is_playing();
    void					go_rewind();
    void					go_back();
    void					go_forward();
    void					goto_frame(int theFrameNum);

    int						get_total_frames();
    int						get_percent_loaded();
    int						get_current_frame();

    std::string				get_current_label(const std::string& theTimeline);

    void					call_frame(const std::string& theTimeline, int theFrameNum);
    void					call_label(const std::string& theTimeline, const std::string& theLabel);

    std::string				get_variable(const std::string& theName);
    void					set_variable(const std::string& theName, const std::string& theValue);

    bool					is_dirty();
    void					set_dirty();

    void					update();
    void					draw(HDC hDC = NULL, bool bClear = TRUE, int x = 0, int y = 0);

    void					mouse_down(int x, int y);
    void					mouse_up(int x, int y);
    void					mouse_move(int x, int y);
    void					mouse_leave();

public:
    void					resize(int theWidth, int theHeight);

    int						get_width();
    int						get_height();

    HDC						get_HDC();
    HBITMAP					get_bitmap();

    COLORREF				get_bg_color();
    void					set_bk_color(COLORREF color);

public:
    EVENT_LOAD_FLASH_LIBRARY	handler_load_flash_library;
    EVENT_UNLOAD_FLASH_LIBRARY	handler_unload_flash_library;

public:
    EVENT_FLASH_COMMAND			event_flash_command;
    EVENT_FLASH_STOPPED			event_flash_stopped;

private:
    friend	class			FlashSink;
    friend	class			FlashControlSite;

protected:
    void					do_cleanup_images();
    void					do_rebuild_images();
    void					do_check_cursor();

    void					do_on_flash_command(const char* szCommand, const char* szArgs);

protected:
    int						m_State;
    HMODULE					m_FlashLibHandle;

    FlashSink*				m_FlashSink;
    FlashControlSite*		m_ControlSite;

    IOleObject*									m_OleObject;
    ShockwaveFlashObjects::IShockwaveFlash*		m_FlashInterface;
    IOleInPlaceObjectWindowless*				m_WindowlessObject;

    int						m_COMCount;
    int						m_PauseCount;
    bool					m_HasLostFocus;

    int						m_nWidth;
    int						m_nHeight;

    long					m_nTotalFrames;
    long					m_nCurrFrame;
    long					m_nLoadingPercent;

    RECT					m_DirtyRect;
    bool					m_FlashDirty;

    HDC						m_hDC;
    HBITMAP					m_hBmp;
    void*					m_pDIB;
    COLORREF				m_bkColor;
};

#endif //__FLASH_PLAYER_H__089BB269_EC67_495d_885A_CAB04219E370__INCLUDED_
