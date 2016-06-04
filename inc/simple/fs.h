#ifndef FS_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define FS_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include <string>
#include <deque>

//	Create dir
bool	FS_ForceCreateDir(const char* pszFullPathFileName );

//	Batch update
struct	FileUpdateItem {
    std::string	target_file;
    std::string	source_file;
};
typedef	std::deque<FileUpdateItem>	FS_UpdateFileList;
bool	FS_UpdateFiles(const FS_UpdateFileList& files);

#endif
