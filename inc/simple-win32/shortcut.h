#ifndef SHORTCUT_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define SHORTCUT_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<string>

//
//	������ݷ�ʽ
//
bool		Shortcut_Create(const char* szPath, const char* szWorkingPath, const char* szLink);
std::string	Shortcut_GetTarget(const char* szLink);
void		Shortcut_Run(const char* szLink, const char* szWorkdir);

#endif
