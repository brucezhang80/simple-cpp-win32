
#include	"shortcut.h"

#include	<Shlwapi.h>
#include	<ShlObj.h>

//
//	������ݷ�ʽ
//
bool	Shortcut_Create(const char* szPath, const char* szWorkingPath, const char* szLink) {
    HRESULT hres ;
    IShellLink * psl ;
    IPersistFile* ppf ;
    WCHAR wsz[MAX_PATH]= {L""};
    ////��ʼ��COM
    CoInitialize (NULL);
    //����һ��IShellLinkʵ��
    hres = CoCreateInstance( CLSID_ShellLink, NULL,CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
    if( FAILED( hres)) {
        CoUninitialize ();
        return false ;
    }

    //����Ŀ��Ӧ�ó���
    psl->SetPath( szPath);
    psl->SetWorkingDirectory(szWorkingPath);

    //��IShellLink��ȡ��IPersistFile�ӿ�
    //���ڱ����ݷ�ʽ�������ļ� (*.lnk)
    hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf) ;
    if( FAILED( hres)) {
        CoUninitialize ();
        return false ;
    }

    // ȷ�������ļ���ΪANSI��ʽ
    MultiByteToWideChar( CP_ACP, 0, szLink, -1, wsz, MAX_PATH) ;

    //����IPersistFile::Save
    //�����ݷ�ʽ�������ļ� (*.lnk)
    hres = ppf->Save(wsz, STGM_READWRITE);
    //�ͷ�IPersistFile��IShellLink�ӿ�
    ppf->Release() ;
    psl->Release() ;
    CoUninitialize();

    return true;
}

std::string	Shortcut_GetTarget(const char* szLink) {
    wchar_t	wszLink[MAX_PATH]	= {};
    ::MultiByteToWideChar(CP_ACP, NULL, szLink, int(strlen(szLink)), wszLink, MAX_PATH);

    char	buf[MAX_PATH]	= {};

    IShellLink   *pShellLink;
    HRESULT hRes;

    ::CoInitialize(NULL);
    hRes = CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void **)&pShellLink);
    if(SUCCEEDED(hRes)) {
        IPersistFile   *ppf;
        hRes = pShellLink->QueryInterface(IID_IPersistFile,(void **)&ppf);
        if(SUCCEEDED(hRes)) {
            hRes = ppf->Load(wszLink, TRUE);
            if(SUCCEEDED(hRes)) {
                pShellLink->GetPath(buf, MAX_PATH, NULL, 0);
            }
            ppf->Release();
        }
        pShellLink->Release();
    }
    ::CoUninitialize();

    return	std::string(buf);
}

void	Shortcut_Run(const char* szLink, const char* szWorkdir) {
    wchar_t	wszLink[MAX_PATH]	= {};
    ::MultiByteToWideChar(CP_ACP, NULL, szLink, int(strlen(szLink)), wszLink, MAX_PATH);

    IShellLink   *pShellLink;
    HRESULT hRes;

    ::CoInitialize(NULL);
    hRes = CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void **)&pShellLink);
    if(SUCCEEDED(hRes)) {
        IPersistFile   *ppf;
        hRes = pShellLink->QueryInterface(IID_IPersistFile,(void **)&ppf);
        if(SUCCEEDED(hRes)) {
            hRes = ppf->Load(wszLink, TRUE);
            if(SUCCEEDED(hRes)) {
                int		show	= SW_SHOW;
                char	path[MAX_PATH]	= {};
                char	workdir[MAX_PATH]	= {};
                pShellLink->GetShowCmd(&show);
                pShellLink->GetPath(path, MAX_PATH, NULL, 0);
                pShellLink->GetWorkingDirectory(workdir, MAX_PATH);
                if(path[0] != 0) {
                    ShellExecute(NULL, "open", path, NULL, workdir, show);
                }
            }
            ppf->Release();
        }
        pShellLink->Release();
    }
    ::CoUninitialize();
}
