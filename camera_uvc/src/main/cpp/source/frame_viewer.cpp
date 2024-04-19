//
// Created by Hsj on 2024/04/03.
//

#include "frame_viewer.h"

NAMESPACE_L

frame_viewer::frame_viewer(uint16_t width, uint16_t height, ANativeWindow *window)
{
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
}

frame_viewer::~frame_viewer()
{

}

void frame_viewer::pause()
{

}

void frame_viewer::update(const uint8_t *data)
{

}

void frame_viewer::update_rgb(const uint8_t *data)
{

}

void frame_viewer::update_i420(const uint8_t *data)
{

}

void frame_viewer::update_rgb_align(const uint8_t *data)
{

}

void frame_viewer::update_i420_align(const uint8_t *data)
{

}

NAMESPACE_R