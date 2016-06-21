﻿#include	<Windows.h>

#include	"subimage.h"

using	namespace	Gdiplus;

void	SubImage_Clear(SubImage* img) {
    img->image	= NULL;
    img->rect	= Gdiplus::Rect();
}

// img.png[, l,r,w,h]
bool	SubImage_Load(SubImage* img, const std::string& param) {
    return	SubImage_Load(img, param, 0);
}

bool	SubImage_Load(SubImage* img, const std::string& param, SubImage_ImageCache* cache) {
    // TODO: ...
    return	false;
}

void	SubImage_Destroy(SubImage* img) {
    if(NULL != img->image) {
        delete	img->image;
        img->image	= NULL;
    }
    img->rect	= Gdiplus::Rect();
}

SubImage_ImageCache::SubImage_ImageCache() {
}

SubImage_ImageCache::~SubImage_ImageCache() {
    this->clear();
}

void	SubImage_ImageCache::clear() {
    DataContainer::const_iterator
    it	= data_.begin(),
     it_end	= data_.end();
    for(; it != it_end; ++it) {
        delete	it->second;
    }
    data_.clear();
}

bool	SubImage_ImageCache::has(const std::string& key) {
    return	data_.end() == data_.find(key);
}

Gdiplus::Image*	SubImage_ImageCache::get(const std::string& key) {
    DataContainer::const_iterator	it	= data_.find(key);
    return	(data_.end() == it) ? 0 : it->second;
}

void	SubImage_ImageCache::set(const std::string& key, Gdiplus::Image* image) {
    if(NULL == image) {
        data_.erase(key);
    } else {
        data_[key]	= image;
    }
}

bool	SubImage_ImageCache::load(SubImage& sub_img, const std::string& param) {
    return	SubImage_Load(&sub_img, param, this);
}
