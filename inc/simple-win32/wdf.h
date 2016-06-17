#ifndef WDF_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define WDF_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	数据文件
//
struct	WDF_DataFileIndex;
class	WDF_DataFile {
public:
    WDF_DataFile() : m_BlockIndex(0), m_hFile(INVALID_HANDLE_VALUE)		{}
    ~WDF_DataFile();

public:
    bool				open(const char *name);
    void				close();
    bool				valid() const {
        return m_hFile != INVALID_HANDLE_VALUE;
    }

public:
    WDF_DataFileIndex*	search_file(DWORD id) const;
    WDF_DataFileIndex*	search_file(const char* filename) const;
    HANDLE				file_handle() const {
        return m_hFile;
    }

protected:
    HANDLE				m_hFile;						// 数据文件句柄

    WDF_DataFileIndex*	m_BlockIndex;					// 内容文件信息
    int					m_BlockNumber;					// 内容文件数量

private:
    WDF_DataFile(const WDF_DataFile&);
    const WDF_DataFile&	operator=(const WDF_DataFile&);
};

//
//	数据文件中的子文件
//
class	WDF_File {
public:
    WDF_File();
    virtual				~WDF_File();

public:
    //
    //	使用方法一：将数据读取到独立内存中
    //

    // 加载一个文件
    bool				load(WDF_DataFile& datafile, const char *filename);
    // 读数据指针
    void*				data() {
        return m_pDataPtr;
    }
    void*				data(int offset) {
        return (void*)((char*)m_pDataPtr + offset);
    }

public:
    //
    //	使用方法二：使用实际数据文件的内容
    //

    // 打开一个文件
    bool				open(WDF_DataFile& datafile, const char *filename);
    // 读数据到指定 Buffer
    bool				read(void* buffer, int s, int offset, int *read = 0);
    // 跳过 s 字节
    void				skip(int s);
    // 读数据到指定 Buffer
    bool				read(void* buffer, int s, int *read = 0);
    // 取文件长度
    DWORD				size() {
        return m_dwDataSize;
    }
    // 取文件指针
    DWORD				tell() const;
    // 关闭文件
    void				close();
    // 是否到文件尾
    bool				eof() const {
        return tell() >= m_dwDataSize;
    }

protected:
    void*				m_pDataPtr;						// 内容指针
    DWORD				m_dwDataSize;					// 内容大小

    HANDLE				m_hFile;						// 数据文件句柄
    DWORD				m_dwOffset;						// 在数据文件中的偏移

private:
    WDF_File(const WDF_File&);
    const WDF_File&	operator=(const WDF_File&);
};

#endif
