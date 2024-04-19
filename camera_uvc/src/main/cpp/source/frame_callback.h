//
// Created by Hsj on 2024/04/03.
//

#pragma once

#include "common.hpp"

NAMESPACE_L

    #include <jni.h>

    class frame_callback {
    public:
        explicit frame_callback(JavaVM *jvm, jobject obj, jmethodID mid, size_t max_size);
        ~frame_callback();
        void update_frame(size_t size);
        void update_picture(size_t size);
    private:
        JavaVM *_jvm;
        jobject _obj;
        jmethodID _mid;
    };

NAMESPACE_R