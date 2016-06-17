#ifndef WDF_PROTOCOL_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define WDF_PROTOCOL_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	"wdf.h"
#include	"ie_protocol.h"

class	WDF_FileSystem;

//
//	IE控件 WDF 协议实现
//
class WDF_Protocol	: public IE_CustomProtocol {
public:
    explicit		WDF_Protocol(bool bAutoDelete = false, bool bZeroRefCount = false);
    virtual			~WDF_Protocol();

public:
    bool			initialize(WDF_FileSystem* pFileSystem, const char* pszProtocolPrefix);

protected:
    virtual	IE_CustomProtocol*	do_create_protocol();
    virtual	void				do_destroy_protocol(IE_CustomProtocol* pProtocol);
    virtual	bool				do_object_load(const char* szUrl);
    virtual	std::wstring		do_object_mime_type(const char* szUrl);
    virtual	size_t				do_object_data_size(const char* szUrl);
    virtual	size_t				do_object_data_read(void *pv, size_t size, size_t offset);

protected:
    WDF_File			m_wdfFile;
    WDF_FileSystem*		m_pFileSystem;
    size_t				m_nPrefixLength;
};

#endif
