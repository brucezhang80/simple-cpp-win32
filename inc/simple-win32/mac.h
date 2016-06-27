#ifndef MAC_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define MAC_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<string>

#include	<Iphlpapi.h>

typedef	bool	(*MAC_ADAPTER_ENUM_FUNC)(PIP_ADAPTER_INFO info, void* user_data);

//	enum the net-cards' info.
bool			Mac_EnumAdapterInfo(MAC_ADAPTER_ENUM_FUNC enum_func, void* user_data);

//	is Physical net card or not.
bool			Mac_IsPhysicalAdapter(PIP_ADAPTER_INFO adapter_info);

std::string		Mac_GetPhysicalAdapterMac();

#endif
