#ifndef STRING_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define STRING_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	"simple/string.h"

inline	bool	String_ToPoint(const std::string& str, POINT& pt) {
    return	string_tonumbers(str, pt.x, pt.y);
}

inline	bool	String_ToSize(const std::string& str, SIZE& size) {
    return	string_tonumbers(str, size.cx, size.cy);
}

inline	bool	String_ToRect(const std::string& str, RECT& rc) {
    return	string_tonumbers(str, rc.left, rc.top, rc.right, rc.bottom);
}

#endif
