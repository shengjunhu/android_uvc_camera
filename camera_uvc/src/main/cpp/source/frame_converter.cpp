//
// Created by Hsj on 2024/04/03.
//

#include <cassert>
#include "frame_converter.h"

NAMESPACE_L

frame_converter::frame_converter(std::shared_ptr<support_info> &info)
{

}

frame_converter::~frame_converter()
{

}

int frame_converter::start()
{
    return STATE_FAILURE;
}

int frame_converter::enqueue(void *data, size_t size)
{
    return STATE_FAILURE;
}

int frame_converter::set_frame_callback(frame_callback *cb, void *user_ptr)
{
    return STATE_FAILURE;
}

int frame_converter::set_picture_callback(frame_callback *cb, void *user_ptr)
{
    return STATE_FAILURE;
}

int frame_converter::start_video()
{
    return STATE_FAILURE;
}

int frame_converter::pause_video()
{
    return STATE_FAILURE;
}

int frame_converter::stop_video()
{
    return STATE_FAILURE;
}

int frame_converter::stop()
{
    return STATE_FAILURE;
}

int frame_converter::destroy()
{
    return STATE_FAILURE;
}

NAMESPACE_R