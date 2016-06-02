#include	<cassert>
#include	<memory>
#include	<windows.h>
#include	<shlwapi.h>

#include	"simple/string.h"

#include	"wdf.h"
#include	"wdf_fs.h"

WDFFileSystem::WDFFileSystem(bool bUseOSFile)
    :	use_os_file_(bUseOSFile) {
}

bool WDFFileSystem::GetUsingOSFile() {
    return	use_os_file_;
}

void WDFFileSystem::SetUsingOSFile(bool bUse) {
    use_os_file_	= bUse;
}

WDFFileSystem::DirectoryIterator WDFFileSystem::FindDataFile(const char* virtual_path, std::string& virtual_file) {
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

void WDFFileSystem::Clear() {
    DirectoryIterator	it		= dirs_.begin();
    DirectoryIterator	it_end	= dirs_.end();
    for(; it != it_end; ++it) {
        delete	it->second;
    }

    dirs_.clear();
}

bool WDFFileSystem::OpenDataFile(const char *virtual_dir, const char* filename) {
    if(NULL == virtual_dir || 0 == virtual_dir[0] || NULL == filename || !::PathFileExists(filename)) {
        return	false;
    }

    assert('/' == virtual_dir[strlen(virtual_dir) - 1]);
    if('/' != virtual_dir[strlen(virtual_dir) - 1]) {
        return	false;
    }
    std::auto_ptr<WDFDataFile>	pDataFile(new WDFDataFile);
    if(!pDataFile->open(filename)) {
        return	false;
    }

    WDFDataFile*	theDataFile	= pDataFile.release();
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

void WDFFileSystem::CloseDataFile(const char *virtual_dir) {
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

bool WDFFileSystem::HasFile(const char *virtual_path) {
    if(use_os_file_ && ::PathFileExists(virtual_path)) {
        return	true;
    }

    WDFFile	file;
    return	OpenFile(virtual_path, file);
}

bool WDFFileSystem::LoadFile(const char *virtual_path, WDFFile& file) {
    if(use_os_file_ && ::PathFileExists(virtual_path)) {
        WDFDataFile	datafile;
        return	file.load(datafile, virtual_path);
    }

    std::string			virtual_file;
    DirectoryIterator	it	= FindDataFile(virtual_path, virtual_file);
    if(it != dirs_.end()) {
        return	file.load(*(it->second), virtual_file.c_str());
    }

    return	false;
}

bool WDFFileSystem::OpenFile(const char *virtual_path, WDFFile& file) {
    if(use_os_file_ && ::PathFileExists(virtual_path)) {
        WDFDataFile	datafile;
        return	file.open(datafile, virtual_path);
    }

    std::string			virtual_file;
    DirectoryIterator	it	= FindDataFile(virtual_path, virtual_file);
    if(it != dirs_.end()) {
        return	file.open(*(it->second), virtual_file.c_str());
    }

    return	false;
}
