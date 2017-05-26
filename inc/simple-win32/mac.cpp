#include	<memory>

#include	<windows.h>

#include	"mac.h"

#include	"simple/string.h"

bool	Mac_EnumAdapterInfo(MAC_ADAPTER_ENUM_FUNC enum_func, void* user_data) {
    IP_ADAPTER_INFO	dummy_mac_info	= {0};

    DWORD	dwLen		= sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO	pAdapterInfo	= &dummy_mac_info;
    if(ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(pAdapterInfo, &dwLen)) {
        pAdapterInfo	= (PIP_ADAPTER_INFO)new char[dwLen];
        if(ERROR_SUCCESS != GetAdaptersInfo(pAdapterInfo, &dwLen)) {
            delete[]	(char*)pAdapterInfo;
            return	false;
        }
    }

    bool	result	= false;
    {
        PIP_ADAPTER_INFO	pInfo	= pAdapterInfo;
        while(NULL != pInfo) {
            if(enum_func(pInfo, user_data)) {
                result	= true;
                break;
            }
            pInfo	= pInfo->Next;
        }
    }

    if(&dummy_mac_info != pAdapterInfo) {
        delete[]	(char*)pAdapterInfo;
    }
    return	result;
}

bool	Mac_IsPhysicalAdapter(PIP_ADAPTER_INFO adapter_info) {
    if(NULL == adapter_info) {
        return	false;
    }

    HKEY	hRoot;
    if(ERROR_SUCCESS != RegOpenKeyExA(
                HKEY_LOCAL_MACHINE,
                "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}",
                0,
                KEY_READ,
                &hRoot)) {
        return	false;
    }

    bool	result_is_physical_adapter	= false;
    {
        char	subKey[MAX_PATH + 1]	= {0};
        DWORD	keyLen	= sizeof(subKey) - 1;
        DWORD	dwIndex	= 0;
        while(RegEnumKeyExA(hRoot, dwIndex, subKey, &keyLen, 0, 0, 0, 0) != ERROR_NO_MORE_ITEMS) {
            keyLen	= sizeof(subKey) - 1;
            dwIndex++;

            HKEY	hSubKey;
            if(ERROR_SUCCESS == RegOpenKeyExA(hRoot, subKey, 0, KEY_READ, &hSubKey)) {
                char	value[MAX_PATH + 1]	= {0};
                DWORD	value_len	= sizeof(value) - 1;
                DWORD	value_type	= 0;
                if(	ERROR_SUCCESS == RegQueryValueExA(hSubKey, "NetCfgInstanceId", NULL, &value_type, (LPBYTE)value, &value_len)
                        &&	0 == _strcmpi(value, adapter_info->AdapterName)
                  ) {
                    memset(value, 0, sizeof(value));
                    value_len	= sizeof(value) - 1;
                    if(	ERROR_SUCCESS == RegQueryValueExA(hSubKey, "Characteristics", NULL, &value_type, (LPBYTE)&value, &value_len)
                            &&	4 == ((*(DWORD*)value) & 4)// 4 -> NCF_PHYSICAL
                      ) {
                        result_is_physical_adapter	= true;
                    }
                }
                RegCloseKey(hSubKey);
            }

            memset(subKey, 0, sizeof(subKey));
            if(result_is_physical_adapter) {
                break;
            }
        }
    }
    RegCloseKey(hRoot);

    return	result_is_physical_adapter;
}

static	bool	Mac_ENUM_IsPhysicalAdapterMac(PIP_ADAPTER_INFO info, void* user_data) {
    if(!Mac_IsPhysicalAdapter(info)) {
        return	false;
    }

    std::string*	mac	= (std::string*)user_data;
    for(UINT i = 0; i < info->AddressLength; ++i) {
        mac->append(string_hex(info->Address[i], false));
    }

    return	true;
}

std::string		Mac_GetPhysicalAdapterMac() {
    std::string	mac;
    Mac_EnumAdapterInfo(Mac_ENUM_IsPhysicalAdapterMac, &mac);
    return	mac;
}
