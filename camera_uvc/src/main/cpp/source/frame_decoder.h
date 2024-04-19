//
// Created by Hsj on 2024/04/03.
//

#pragma once

#include "common.hpp"

NAMESPACE_L

    class frame_decoder
    {
    public:
        frame_decoder(uint16_t width, uint16_t height, frame_format format, pixel_format pixel);
        ~frame_decoder();
        int void_fun(uint8_t  *src, size_t size, uint8_t *dst);
        int frame_to_copy(uint8_t *src, size_t size, uint8_t *dst);
        int yuy2_to_rgb(uint8_t  *src, size_t size, uint8_t *dst);
        int yuy2_to_nv21(uint8_t *src, size_t size, uint8_t *dst);
        int yuy2_to_yuv420p(uint8_t *src, size_t size, uint8_t *dst);
        int yuy2_to_yuv422p(uint8_t *src, size_t size, uint8_t *dst);
        int mjpg_to_rgb(uint8_t  *src, size_t size, uint8_t *dst);
        int mjpg_to_nv21(uint8_t *src, size_t size, uint8_t *dst);
        int mjpg_to_yuv420p(uint8_t *src, size_t size, uint8_t *dst);
        int mjpg_to_yuv422p(uint8_t *src, size_t size, uint8_t *dst);
        int h264_to_rgb(uint8_t  *src, size_t size, uint8_t *dst);
        int h264_to_nv21(uint8_t *src, size_t size, uint8_t *dst);
        int h264_to_yuv420p(uint8_t *src, size_t size, uint8_t *dst);
        int h264_to_yuv422p(uint8_t *src, size_t size, uint8_t *dst);

    private:
        // Frame size
        uint16_t width;
        uint16_t height;

        // YUV: length
        uint32_t length_y;
        uint32_t length_u;
        // YUV: stride
        uint32_t stride_rgb;
        uint32_t stride_argb;
        uint32_t stride_yuy2;
        uint16_t stride_uv;
        uint16_t stride_y;
        uint16_t stride_u;
        uint16_t stride_v;

        // YUV: buffer
        uint8_t* buffer;
        uint8_t* buffer_u;
        uint8_t* buffer_v;
        uint8_t* buffer_uv;

        // jpeg-turbo
        int _width;
        int _height;
        int sub_sample;
        int color_space;
        //tjhandle handle;
    };

NAMESPACE_R