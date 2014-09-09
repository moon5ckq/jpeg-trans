/*
 * Author: mlckq <moon5ckq@gmail.com>
 * File: jpeg_trans.cpp
 * Create Date: 2014-09-05 12:35
 */

#include "jpeg_trans.h"

#define BLOCK_SIZE 16384
#define QUALITY 80

using std::vector;
struct DestWithBuff {
    jpeg_destination_mgr dest;
    BuffType *buff;
};

BuffType compress_to_data(const GrayImage& img_data) {
    return compress_to_data(&img_data.data[0], img_data.width, img_data.height);
}

BuffType compress_to_data(JOCTET const* img_data, int width, int height) {
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    BuffType buff;
    JSAMPROW row_pointer[1];
    int row_stride;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 1;
    cinfo.in_color_space = JCS_GRAYSCALE;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 80, true);

    DestWithBuff *dest = (DestWithBuff*)
        (* cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_PERMANENT, sizeof(DestWithBuff));
    dest->buff = &buff;

    cinfo.dest = (jpeg_destination_mgr*) dest;
    cinfo.dest->init_destination =  [](j_compress_ptr c) {
        BuffType &buff = * ((DestWithBuff*) c->dest)->buff;
        buff.resize(BLOCK_SIZE);
        c->dest->next_output_byte = buff.data();
        c->dest->free_in_buffer = buff.size();
    };
    cinfo.dest->empty_output_buffer = [](j_compress_ptr c) -> boolean {
        BuffType &buff = * ((DestWithBuff*) c->dest)->buff;
        size_t old_size = buff.size();
        buff.resize(old_size + BLOCK_SIZE);
        c->dest->next_output_byte = buff.data() + old_size;
        c->dest->free_in_buffer = buff.size() - old_size;
        return true;
    };
    cinfo.dest->term_destination = [](j_compress_ptr c) {
        BuffType &buff = * ((DestWithBuff*) c->dest)->buff;
        buff.resize(buff.size() - c->dest->free_in_buffer);
    };
    jpeg_start_compress(&cinfo, true);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = const_cast<JOCTET*>(&img_data[cinfo.next_scanline * width]);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return buff;
}

GrayImage decompress_from_data(const BuffType& jpg_data) {
    return decompress_from_data(&jpg_data[0], jpg_data.size());
}

GrayImage decompress_from_data(JOCTET const* jpg_data, size_t size) {
    GrayImage img;
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    JSAMPARRAY buffer;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    cinfo.src = (jpeg_source_mgr *)
        (* cinfo.mem->alloc_small) ((j_common_ptr)&cinfo, JPOOL_PERMANENT, sizeof(jpeg_source_mgr));
    cinfo.src->init_source = [](j_decompress_ptr c) { };
    cinfo.src->fill_input_buffer = [](j_decompress_ptr c) -> boolean {
        return true;
    };
    cinfo.src->skip_input_data = [](j_decompress_ptr c, long num_bytes) {
        if (num_bytes > 0) {
            c->src->next_input_byte += num_bytes;
            c->src->bytes_in_buffer -= num_bytes;
        }
    };
    cinfo.src->term_source = [](j_decompress_ptr c) {};
    cinfo.src->resync_to_restart = jpeg_resync_to_restart;
    cinfo.src->bytes_in_buffer = size;
    cinfo.src->next_input_byte = const_cast<JOCTET*>(jpg_data);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    img.width = cinfo.output_width;
    img.height = cinfo.output_height;
    img.channel = cinfo.output_components;
    img.data.resize(img.width * img.height * img.channel);

    int stride = img.width * img.channel;
    buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, stride, 1);

    for (int i = 0; cinfo.output_scanline < cinfo.output_height; ++i) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(img.data.data() + i * stride, buffer[0], stride);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return img;
}

