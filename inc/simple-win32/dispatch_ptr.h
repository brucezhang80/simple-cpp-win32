#ifndef DISPATCHPTR_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define DISPATCHPTR_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	DispatchPtr htmldoc = ...;
//
//	_bstr_t html = htmldoc.Get("body").Get("innerHTML");
//	htmldoc.Set("title", "New Title");
//	htmldoc.Get("body").Get("firstChild").Invoke(
//		"insertAdjacentText", "afterBegin", "hello world");
//

#define		CDispatchPtr		DispatchPtr
#define		CDispatchVariant	DispatchVariant
#define		CDispatchFunctions	DispatchFunctions

#include	"_third/CDispatchPtr/DispatchPtr.h"

#endif
