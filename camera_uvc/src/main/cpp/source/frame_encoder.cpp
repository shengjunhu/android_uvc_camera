//
// Created by Hsj on 2024/04/03.
//

#include "frame_encoder.h"

NAMESPACE_L

frame_encoder::frame_encoder(uint16_t width, uint16_t height,
        pixel_format pixel, video_format format)
{

}

frame_encoder::~frame_encoder()
{

}

int rgb_to_mp4(uint8_t  *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int nv21_to_mp4(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int yuy2_to_mp4(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int rgb_to_h264(uint8_t  *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int nv21_to_h264(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int yuy2_to_h264(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int h264_to_copy(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

NAMESPACE_R