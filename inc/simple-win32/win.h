#ifndef WIN_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define WIN_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	判断是否64位系统
//
bool	Win_Is64bitsSystem();

//
//	根据类名获取子窗口，同 FindWindowEx
//
HWND	Win_FindChildByClass(HWND hWnd, const char* sClass);

#endif
