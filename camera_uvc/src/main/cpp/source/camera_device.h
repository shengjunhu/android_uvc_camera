//
// Created by Hsj on 2024/04/03.
//

#pragma once

#include "common.hpp"

NAMESPACE_L

    #include "libuvc/libuvc.h"
    #include "frame_converter.h"

    class camera_uvc
    {
    public:
        camera_uvc();
        ~camera_uvc();
        int open_device(const std::string fs, const int fd);
        int get_support(std::vector<support_info> &list);
        int set_support(support_info &info);
        int set_callback(frame_callback cb);
        int set_preview(ANativeWindow *win);
        int start_stream();
        int take_picture();
        int start_video();
        int pause_video(bool pause);
        int stop_video();
        int stop_stream();
        int close_device();
        int destroy();
    private:
        // camera
        int _fd;
        std::mutex _mutex;
        volatile camera_state _state;
        std::shared_ptr<support_info> _info;
        std::shared_ptr<frame_converter> _converter;

        // uvc
        uvc_device_t *_device;
        uvc_context_t *_context;
        uvc_stream_ctrl_t _stream_ctrl;
        uvc_device_handle_t *_device_handle;

        inline camera_state get_state() const { return this->_state; }
        inline void set_state(const camera_state state) { this->_state = state; }
    };

NAMESPACE_R