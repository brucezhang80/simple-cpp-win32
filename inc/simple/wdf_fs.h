#ifndef WDF_FS_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define WDF_FS_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<map>
#include	<string>

//
//	WDF文件组成的文件系统
//
class	WDFFile;
class	WDFDataFile;
class	WDFFileSystem {
public:
    explicit	WDFFileSystem(bool bUseOSFile = true);

public:
    void		Clear();

    bool		OpenDataFile(const char *virtual_dir, const char* filename);
    void		CloseDataFile(const char* virtual_dir);

    bool		HasFile(const char *virtual_path);
    bool		LoadFile(const char *virtual_path, WDFFile& file);
    bool		OpenFile(const char *virtual_path, WDFFile& file);

public:
    bool		GetUsingOSFile();
    void		SetUsingOSFile(bool bUse);

private:
    typedef		std::map<std::string, WDFDataFile*, std::greater<std::string> >		DirectoryList;
    typedef		DirectoryList::iterator												DirectoryIterator;

private:
    DirectoryIterator	FindDataFile(const char* virtual_path, std::string& virtual_file);

private:
    DirectoryList		dirs_;
    bool				use_os_file_;
};

#endif	//WDF_FILE_SYSTEM_H__089BB269_EC67_495d_885A_CAB04219E370__INCLUDED_
