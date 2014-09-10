/*
 * Author: mlckq <moon5ckq@gmail.com>
 * File: jpeg_trans.h
 * Create Date: 2014-09-05 12:32
 */

#ifndef __JPEG_TRANS_H__
#define __JPEG_TRANS_H__

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <jpeglib.h>

typedef std::vector<JOCTET> BuffType;
struct GrayImage {
    BuffType data;
    int width, height, channel;
};

BuffType compress_to_data(const GrayImage& img_data);
BuffType compress_to_data(JOCTET const* img_data, int width, int height);
BuffType compress_yuv420(JOCTET const* yuv_data, int width, int height);
GrayImage decompress_from_data(const BuffType& jpg_data);
GrayImage decompress_from_data(JOCTET const* jpg_data, size_t size);

#endif /* !__JPEG_TRANS_H__ */

