#include	<cassert>
#include	<memory>
#include	<windows.h>
#include	<shlwapi.h>

#include	"simple/string.h"

#include	"wdf.h"
#include	"wdf_fs.h"

WDF_FileSystem::WDF_FileSystem(bool bUseOSFile)
    :	use_os_file_(bUseOSFile) {
}

bool WDF_FileSystem::GetUsingOSFile() {
    return	use_os_file_;
}

void WDF_FileSystem::SetUsingOSFile(bool bUse) {
    use_os_file_	= bUse;
}

WDF_FileSystem::DirectoryIterator WDF_FileSystem::FindDataFile(const char* virtual_path, std::string& virtual_file) {
    if(NULL == virtual_path) {
        return	dirs_.end();
    }

    std::string		sKey(virtual_path);
    {
        string_tolower(sKey);
    }

    DirectoryIterator	it	= dirs_.begin();
    for(; it != dirs_.end(); ++it) {
        if(		sKey.size() > it->first.size() + 1
                &&	'/' == sKey[it->first.size() - 1]
                &&	0 == strncmp(sKey.c_str(), it->first.c_str(), it->first.size())
          ) {
            if(NULL != it->second->search_file(sKey.c_str() + it->first.size() - 1)) {
                virtual_file.assign(sKey.c_str() + it->first.size() - 1);
                return	it;
            }
        }
    }

    return	dirs_.end();
}

void WDF_FileSystem::Clear() {
    DirectoryIterator	it		= dirs_.begin();
    DirectoryIterator	it_end	= dirs_.end();
    for(; it != it_end; ++it) {
        delete	it->second;
    }

    dirs_.clear();
}

bool WDF_FileSystem::OpenDataFile(const char *virtual_dir, const char* filename) {
    if(NULL == virtual_dir || 0 == virtual_dir[0] || NULL == filename || !::PathFileExists(filename)) {
        return	false;
    }

    assert('/' == virtual_dir[strlen(virtual_dir) - 1]);
    if('/' != virtual_dir[strlen(virtual_dir) - 1]) {
        return	false;
    }
    std::auto_ptr<WDF_DataFile>	pDataFile(new WDF_DataFile);
    if(!pDataFile->open(filename)) {
        return	false;
    }

    WDF_DataFile*	theDataFile	= pDataFile.release();
    std::string		sKey(virtual_dir);
    {
        string_tolower(sKey);
    }

    DirectoryIterator	it	= dirs_.find(sKey);
    if(dirs_.end() != it) {
        delete	it->second;
        it->second	= theDataFile;
        return	true;
    }

    dirs_.insert(std::make_pair(sKey, theDataFile));
    return	true;
}

void WDF_FileSystem::CloseDataFile(const char *virtual_dir) {
    if(NULL == virtual_dir) {
        return;
    }

    std::string		sKey(virtual_dir);
    {
        string_tolower(sKey);
    }

    DirectoryIterator	it	= dirs_.find(sKey);
    if(dirs_.end() != it) {
        delete	it->second;
        dirs_.erase(it);
    }
}

bool WDF_FileSystem::HasFile(const char *virtual_path) {
    if(use_os_file_ && ::PathFileExists(virtual_path)) {
        return	true;
    }

    WDF_File	file;
    return	OpenFile(virtual_path, file);
}

bool WDF_FileSystem::LoadFile(const char *virtual_path, WDF_File& file) {
    if(use_os_file_ && ::PathFileExists(virtual_path)) {
        WDF_DataFile	datafile;
        return	file.load(datafile, virtual_path);
    }

    std::string			virtual_file;
    DirectoryIterator	it	= FindDataFile(virtual_path, virtual_file);
    if(it != dirs_.end()) {
        return	file.load(*(it->second), virtual_file.c_str());
    }

    return	false;
}

bool WDF_FileSystem::OpenFile(const char *virtual_path, WDF_File& file) {
    if(use_os_file_ && ::PathFileExists(virtual_path)) {
        WDF_DataFile	datafile;
        return	file.open(datafile, virtual_path);
    }

    std::string			virtual_file;
    DirectoryIterator	it	= FindDataFile(virtual_path, virtual_file);
    if(it != dirs_.end()) {
        return	file.open(*(it->second), virtual_file.c_str());
    }

    return	false;
}
