#include <cassert>
#include <windows.h>
#include <shlwapi.h>

#include "fs.h"
#define		LOG_LOG(MSG)

static	const std::string	GS_UPDATE_BACKUP_POSTFIX = ".update_backup";

static	bool Static_CreateDir( const char* pszDir ) {
    assert(pszDir);
    bool bRet;
    bRet = (0 != CreateDirectory( pszDir, NULL ));
    if(!bRet) {
        if( ERROR_ALREADY_EXISTS == GetLastError() )
            return true;
    }
    return bRet;
}


bool FS_ForceCreateDir(const char* pszFullPathFileName ) {
    char  Dir[_MAX_PATH+1];
    int	 nNeed;
    const char * p, * pLast;
    bool  Result;

    Result=true;
    assert(pszFullPathFileName);
    pLast=pszFullPathFileName;
    if(strlen(pLast)>_MAX_PATH) return FALSE;
    while(NULL!=*pLast) {
        p=strstr(pLast,"\\");
        if(NULL==p) {
            return Result;
        }
        nNeed=p-pszFullPathFileName;
        if(nNeed>0) {
            memset(Dir,0,sizeof(Dir));
            strncpy(Dir,pszFullPathFileName,p-pszFullPathFileName);
            Result = (0 != Static_CreateDir(Dir));
        }
        p++;
        pLast=p;
    }
    return Result;
}

static	void	UpdateFiles_RollbackBackupFiles(const FS_UpdateFileList& files) {
    LOG_LOG("���� Rollback �ļ����²���");

    FS_UpdateFileList::const_iterator	it, it_end;

    // ȥ���ļ���׺��
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        // ԭʼ�ļ���������Ŀ���ļ����ڣ�˵���Ѿ������˸��²������ָ�Դ�ļ���
        if( PathFileExists(it->target_file.c_str()) && !PathFileExists(it->source_file.c_str()) ) {
            LOG_LOG("���� Rollback �ļ�["+it->source_file+"]");

            int ret = rename( it->target_file.c_str(), it->source_file.c_str() );
            assert( 0 == ret );// �������ʱ��ʧ�ܣ�ҲӦ�ü���ѭ����
        }
        // �����ļ�������Ŀ���ļ������ڣ���Ҫ�ָ�Ŀ���ļ���
        const std::string backup_file = it->target_file + GS_UPDATE_BACKUP_POSTFIX;
        if( PathFileExists(backup_file.c_str()) && !PathFileExists(it->target_file.c_str()) ) {
            LOG_LOG("���� Rollback �ļ�["+it->target_file+"]");

            int ret = rename( backup_file.c_str(), it->target_file.c_str() );
            assert( 0 == ret );// �������ʱ��ʧ�ܣ�ҲӦ�ü���ѭ����
        }
    }
}

static	void	UpdateFiles_DeleteBackupFiles(const FS_UpdateFileList& files) {
    FS_UpdateFileList::const_iterator	it, it_end;

    // ȥ���ļ���׺��
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        const std::string backup_file = it->target_file + GS_UPDATE_BACKUP_POSTFIX;
        if( PathFileExists(backup_file.c_str()) ) {
            LOG_LOG("����ɾ����ʱ�ļ�["+backup_file+"]");

            int ret = remove(backup_file.c_str());
            assert( 0 == ret );// �������ʱ��ʧ�ܣ�ҲӦ�ü���ѭ����
        }
    }
}

bool	FS_UpdateFiles(const FS_UpdateFileList& files) {
    LOG_LOG("��ʼִ���ļ����²���");

    FS_UpdateFileList::const_iterator	it, it_end;

    // ���ԭʼ�ļ���ȷ��
    LOG_LOG("���ڼ�������صĸ����ļ���...");
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        if( !PathFileExists(it->source_file.c_str()) ) {
            LOG_LOG("���� ��ʱ�ļ�["+it->source_file+"]������");
            return false;
        }
    }

    // ���ݴ����µ��ļ��������ļ����֣�
    LOG_LOG("���ڱ���Ҫ�����µ��ļ�...");
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        if( PathFileExists(it->target_file.c_str()) ) {
            // ����Ŀ���ļ�
            std::string backup_file = it->target_file + GS_UPDATE_BACKUP_POSTFIX;
            if( ::PathFileExists(backup_file.c_str()) ) {
                ::remove(backup_file.c_str());
            }
            if( 0 != rename(it->target_file.c_str(), backup_file.c_str()) ) {
                UpdateFiles_RollbackBackupFiles(files);
                return false;
            }
        }
    }

    // ִ���ļ����²���
    LOG_LOG("����ִ���ļ�����...");
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        LOG_LOG("���ڸ����ļ�["+it->target_file+"]");
        // ���ȸ����ļ���Ŀ��·��
        FS_ForceCreateDir(it->target_file.c_str());
        if( 0 == ::MoveFile(it->source_file.c_str(), it->target_file.c_str()) ) {
            LOG_LOG("�����ļ�["+it->target_file+"]����ʧ��");
            UpdateFiles_RollbackBackupFiles(files);
            return false;
        }
    }

    // ɾ�����ݵ��ļ�
    LOG_LOG("����ɾ�����ݵ��ļ�...");
    UpdateFiles_DeleteBackupFiles(files);

    LOG_LOG("�ļ����³ɹ����");
    return true;
}
