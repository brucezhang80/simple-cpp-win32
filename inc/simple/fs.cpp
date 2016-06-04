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
    LOG_LOG("正在 Rollback 文件更新操作");

    FS_UpdateFileList::const_iterator	it, it_end;

    // 去掉文件后缀名
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        // 原始文件不存在且目标文件存在，说明已经进行了更新操作，恢复源文件。
        if( PathFileExists(it->target_file.c_str()) && !PathFileExists(it->source_file.c_str()) ) {
            LOG_LOG("正在 Rollback 文件["+it->source_file+"]");

            int ret = rename( it->target_file.c_str(), it->source_file.c_str() );
            assert( 0 == ret );// 即便这个时候失败，也应该继续循环。
        }
        // 备份文件存在且目标文件不存在，需要恢复目标文件。
        const std::string backup_file = it->target_file + GS_UPDATE_BACKUP_POSTFIX;
        if( PathFileExists(backup_file.c_str()) && !PathFileExists(it->target_file.c_str()) ) {
            LOG_LOG("正在 Rollback 文件["+it->target_file+"]");

            int ret = rename( backup_file.c_str(), it->target_file.c_str() );
            assert( 0 == ret );// 即便这个时候失败，也应该继续循环。
        }
    }
}

static	void	UpdateFiles_DeleteBackupFiles(const FS_UpdateFileList& files) {
    FS_UpdateFileList::const_iterator	it, it_end;

    // 去掉文件后缀名
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        const std::string backup_file = it->target_file + GS_UPDATE_BACKUP_POSTFIX;
        if( PathFileExists(backup_file.c_str()) ) {
            LOG_LOG("正在删除临时文件["+backup_file+"]");

            int ret = remove(backup_file.c_str());
            assert( 0 == ret );// 即便这个时候失败，也应该继续循环。
        }
    }
}

bool	FS_UpdateFiles(const FS_UpdateFileList& files) {
    LOG_LOG("开始执行文件更新操作");

    FS_UpdateFileList::const_iterator	it, it_end;

    // 检查原始文件正确性
    LOG_LOG("正在检查已下载的更新文件包...");
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        if( !PathFileExists(it->source_file.c_str()) ) {
            LOG_LOG("错误： 临时文件["+it->source_file+"]不存在");
            return false;
        }
    }

    // 备份待更新的文件（更改文件名字）
    LOG_LOG("正在备份要被更新的文件...");
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        if( PathFileExists(it->target_file.c_str()) ) {
            // 备份目标文件
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

    // 执行文件更新操作
    LOG_LOG("正在执行文件更新...");
    for( it = files.begin(), it_end = files.end(); it != it_end; ++it ) {
        LOG_LOG("正在更新文件["+it->target_file+"]");
        // 首先复制文件到目标路径
        FS_ForceCreateDir(it->target_file.c_str());
        if( 0 == ::MoveFile(it->source_file.c_str(), it->target_file.c_str()) ) {
            LOG_LOG("错误：文件["+it->target_file+"]更新失败");
            UpdateFiles_RollbackBackupFiles(files);
            return false;
        }
    }

    // 删除备份的文件
    LOG_LOG("正在删除备份的文件...");
    UpdateFiles_DeleteBackupFiles(files);

    LOG_LOG("文件更新成功完成");
    return true;
}
