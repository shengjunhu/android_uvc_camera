//
// Created by Hsj on 2024/04/03.
//

#include "frame_decoder.h"

NAMESPACE_L

frame_decoder::frame_decoder(uint16_t width, uint16_t height,
        frame_format format, pixel_format pixel)
{

}

frame_decoder::~frame_decoder()
{

}

int frame_decoder::void_fun(uint8_t  *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::frame_to_copy(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::yuy2_to_rgb(uint8_t  *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::yuy2_to_nv21(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::yuy2_to_yuv420p(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::yuy2_to_yuv422p(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::mjpg_to_rgb(uint8_t  *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::mjpg_to_nv21(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::mjpg_to_yuv420p(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::mjpg_to_yuv422p(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}


int frame_decoder::h264_to_rgb(uint8_t  *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::h264_to_nv21(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::h264_to_yuv420p(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

int frame_decoder::h264_to_yuv422p(uint8_t *src, size_t size, uint8_t *dst)
{
    return STATE_FAILURE;
}

NAMESPACE_R