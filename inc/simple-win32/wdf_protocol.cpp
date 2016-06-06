
#include	<Windows.h>

#include	"wdf.h"
#include	"wdf_fs.h"
#include	"wdf_protocol.h"

WDF_Protocol::WDF_Protocol(bool bAutoDelete, bool bZeroRefCount)
    : IE_CustomProtocol(bAutoDelete, bZeroRefCount)
    , m_pFileSystem(NULL)
    , m_nPrefixLength(0) {
}

WDF_Protocol::~WDF_Protocol() {
}

bool	WDF_Protocol::initialize(WDF_FileSystem* pFileSystem, const char* pszProtocolPrefix) {
    if(NULL == pszProtocolPrefix) {
        return	false;
    }

    m_pFileSystem	= pFileSystem;
    m_nPrefixLength	= strlen(pszProtocolPrefix);
    return	(NULL != m_pFileSystem);
}

IE_CustomProtocol*	WDF_Protocol::do_create_protocol() {
    WDF_Protocol*	pObject	= new WDF_Protocol(true, true);
    pObject->m_nPrefixLength	= this->m_nPrefixLength;
    pObject->m_pFileSystem		= this->m_pFileSystem;
    return	pObject;
}

void	WDF_Protocol::do_destroy_protocol(IE_CustomProtocol* pProtocol) {
    delete	pProtocol;
}

bool	WDF_Protocol::do_object_load(const char* szUrl) {
    m_wdfFile.close();
    if(		NULL != m_pFileSystem
            &&	m_pFileSystem->LoadFile(szUrl + m_nPrefixLength, m_wdfFile)
      ) {
        return	true;
    }

    return	false;
}

std::wstring	WDF_Protocol::do_object_mime_type(const char* szUrl) {
    return	IE_CustomProtocol::do_object_mime_type(szUrl);
}

size_t	WDF_Protocol::do_object_data_size(const char* szUrl) {
    return	m_wdfFile.size();
}

size_t	WDF_Protocol::do_object_data_read(void *pv, size_t size, size_t offset) {
    int	nReaded	= 0;
    m_wdfFile.read(pv, size, offset, &nReaded);
    return	nReaded;
}
