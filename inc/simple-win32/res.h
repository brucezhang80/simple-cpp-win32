#ifndef RES_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define RES_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

bool	Res_ReplaceICO		(const char* pszModule, const char* sResID, const char* lpIconFile);
bool	Res_ReplaceString	(const char* pszModule, int nResID, const char* pszText);

//
//	获取资源内容
//
bool	Res_LoadResource(
    HMODULE		hModule,
    const char*	res_name,
    const char*	res_type,
    void*&		res_data,
    size_t&		res_size
);

//
//	获取文件内容(必须确保空间足够大)
//
bool	Res_LoadFile(
    const char*	file_name,
    void*		res_data,
    size_t&		res_size
);

#endif
