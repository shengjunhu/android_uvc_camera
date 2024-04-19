//
// Created by Hsj on 2024/04/03.
//

#pragma once

#include "common.hpp"

NAMESPACE_L

    #include <android/native_window_jni.h>

    class frame_viewer {
    public:
        explicit frame_viewer(ANativeWindow *window, uint16_t width, uint16_t height);
        ~frame_viewer();
        void pause();
        void update(const uint8_t *data);
    private:
        void update_rgb(const uint8_t *data);
        void update_i420(const uint8_t *data);
        void update_rgb_align(const uint8_t *data);
        void update_i420_align(const uint8_t *data);

        uint16_t _width;
        uint16_t _height;
        ANativeWindow *_window;

        uint32_t length_y;
        uint32_t length_u;

        uint32_t stride_y;
        uint32_t stride_u;
        uint32_t stride_v;
    };

NAMESPACE_R