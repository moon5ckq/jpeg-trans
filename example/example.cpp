/*
 * Author: mlckq <moon5ckq@gmail.com>
 * File: main.cpp
 * Create Date: 2014-09-05 12:47
 */

#include <cstdio>
#include <cstdlib>
#include "jpeg_trans.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: %s <output> [width height] [yuv_data]\n", argv[0]);
        return 0;
    }


    int width = argc > 3 ? atoi(argv[2]) : 320,
        height = argc > 3 ? atoi(argv[3]) : 240;
 
    if (argc > 4) {
        FILE *fin = fopen(argv[4] , "rb");
        fseek(fin , 0 , SEEK_END);
        int len = ftell(fin);
        rewind(fin);
        JOCTET* yuv_data = new JOCTET[len];
        printf("yuv_data size = %d\n", len);
        fread(yuv_data, sizeof(JOCTET), len, fin);
        fclose(fin);

        auto ret = compress_yuv420(yuv_data, width, height);

        // save to file
        FILE* out = fopen(argv[1], "wb");
        fwrite(ret.data(), sizeof(JOCTET), ret.size(), out);
        fclose(out);

        return 0;
    }
    
    JOCTET * data = new JOCTET[width * height];
    // get random gray image
    for (int i = 0; i < width * height; ++i)
        data[i] = rand() & 0xff;

    // compress
    auto ret = compress_to_data(data, width, height);
    printf("from [%dx%d] %d bytes, compress to %lu bytes\n",
        width, height, width * height, ret.size());

    // decompress
    auto img = decompress_from_data(ret);
    // let random range change to [128, 255]
    for (int i = 0; i < img.width * img.height * img.channel; ++i)
        if (img.data[i] < 128) img.data[i] = 255 - img.data[i];
    printf("from %lu bytes, decompress to [%dx%d] %lu bytes\n",
        ret.size(), img.width, img.height, img.data.size());

    // compress
    auto ret2 = compress_to_data(img);
    printf("from [%dx%d] %d bytes, compress to %lu bytes\n",
        width, height, width * height, ret2.size());

    // save to file
    FILE* out = fopen(argv[1], "wb");
    fwrite(ret2.data(), sizeof(JOCTET), ret2.size(), out);
    fclose(out);

    return 0;
}

