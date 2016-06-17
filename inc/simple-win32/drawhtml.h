#ifndef DRAW_HTML_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define DRAW_HTML_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

/* DrawHTML()
 * Drop-in replacement for DrawText() supporting a tiny subset of HTML.
 */

#if defined __cplusplus
extern "C"
#endif
int __stdcall DrawHTML(
    HDC		hdc,		// handle of device context
    LPCTSTR	lpString,	// address of string to draw
    int		nCount,		// string length, in characters
    LPRECT	lpRect,		// address of structure with formatting dimensions
    UINT	uFormat,	// text-drawing flags
    UINT	uLineSpace = 0		// 行间空白高度
);
#endif
