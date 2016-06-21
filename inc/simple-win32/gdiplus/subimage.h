#ifndef SUBIMAGE_H_WIN_GDIPLUS_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define SUBIMAGE_H_WIN_GDIPLUS_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	<map>
#include	<string>

#include	<Gdiplus.h>

struct	SubImage;
class	SubImage_ImageCache;

//
//	子位图，即取Image的一部分
//
struct	SubImage {
    Gdiplus::Image*	image;
    Gdiplus::Rect	rect;
};

void	SubImage_Clear(SubImage*);
bool	SubImage_Load(SubImage*, const std::string&);	// img.png[, l,r,w,h]
bool	SubImage_Load(SubImage*, const std::string&, SubImage_ImageCache* cache);
void	SubImage_Destroy(SubImage*);

//
//	子位图的Image缓存
//
class	SubImage_ImageCache {
public:
    SubImage_ImageCache();
    ~SubImage_ImageCache();

public:
    void			clear();

    bool			has(const std::string& key);
    Gdiplus::Image*	get(const std::string& key);
    void			set(const std::string& key, Gdiplus::Image* image);

    bool			load(SubImage& sub_img, const std::string& param);

private:
    typedef		std::map<std::string, Gdiplus::Image*>	DataContainer;
    DataContainer	data_;
};

#endif
