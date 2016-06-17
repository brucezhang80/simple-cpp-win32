#ifndef DESKTOP_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define DESKTOP_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	获取桌面SHELLDLL DefView窗口句柄
//
HWND	Desktop_FindSHELLDLLDefView(const char* sClass);

//
//	获取桌面SysListView窗口句柄
//
HWND	Desktop_GetSysListView();

//
//	获取桌面图标矩形
//
bool	Desktop_GetIconRect(const char* psCaption, RECT* pRect);


#endif
