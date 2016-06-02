#ifndef WDF_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define WDF_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	数据文件
//
struct	WDFDataFileIndex;
class	WDFDataFile {
public:
    WDFDataFile() : m_BlockIndex(0), m_hFile(INVALID_HANDLE_VALUE)		{}
    ~WDFDataFile();

public:
    bool				open(const char *name);
    void				close();
    bool				valid() const {
        return m_hFile != INVALID_HANDLE_VALUE;
    }

public:
    WDFDataFileIndex*	search_file(DWORD id) const;
    WDFDataFileIndex*	search_file(const char* filename) const;
    HANDLE				file_handle() const {
        return m_hFile;
    }

protected:
    HANDLE				m_hFile;						// 数据文件句柄

    WDFDataFileIndex*	m_BlockIndex;					// 内容文件信息
    int					m_BlockNumber;					// 内容文件数量

private:
    WDFDataFile(const WDFDataFile&);
    const WDFDataFile&	operator=(const WDFDataFile&);
};

//
//	数据文件中的子文件
//
class	WDFFile {
public:
    WDFFile();
    virtual				~WDFFile();

public:
    //
    //	使用方法一：将数据读取到独立内存中
    //

    // 加载一个文件
    bool				load(WDFDataFile& datafile, const char *filename);
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
    bool				open(WDFDataFile& datafile, const char *filename);
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
    WDFFile(const WDFFile&);
    const WDFFile&	operator=(const WDFFile&);
};

#endif	//WDF_FILE_H__089BB269_EC67_495d_885A_CAB04219E370__INCLUDED_
