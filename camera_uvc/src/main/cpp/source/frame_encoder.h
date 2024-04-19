//
// Created by Hsj on 2024/04/03.
//

#pragma once

#include "common.hpp"

NAMESPACE_L

    typedef enum
    {
        VIDEO_FORMAT_NONE    = 0x00,
        VIDEO_FORMAT_H264    = 0x10,
        VIDEO_FORMAT_MP4     = 0x11,
    } video_format;

    class frame_encoder
    {
    public:
        frame_encoder(uint16_t width, uint16_t height, pixel_format pixel, video_format format);
        ~frame_encoder();
        int rgb_to_mp4(uint8_t *src, size_t size, uint8_t *dst);
        int nv21_to_mp4(uint8_t *src, size_t size, uint8_t *dst);
        int yuy2_to_mp4(uint8_t *src, size_t size, uint8_t *dst);
        int rgb_to_h264(uint8_t *src, size_t size, uint8_t *dst);
        int nv21_to_h264(uint8_t *src, size_t size, uint8_t *dst);
        int yuy2_to_h264(uint8_t *src, size_t size, uint8_t *dst);
        int h264_to_copy(uint8_t *src, size_t size, uint8_t *dst);

    private:

    };
NAMESPACE_R
