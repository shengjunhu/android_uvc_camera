//
// Created by Hsj on 2024/04/03.
//

#include "common.hpp"
#include "frame_callback.h"

NAMESPACE_L

frame_callback::frame_callback(JavaVM *jvm, jobject obj, jmethodID mid, size_t max_size):
        _jvm(jvm), _mid(mid) {
    LOG_INTO();
    _obj = env->NewGlobalRef(obj);
    LOG_EXIT();
}

frame_callback::~frame_callback() {
    LOG_INTO();
    if (_jvm != nullptr && _obj != nullptr) {
        JNIEnv *env;
        if (JNI_OK == _jvm->GetEnv((void **) &env, JNI_VERSION_1_6)) {
            env->DeleteGlobalRef(_obj);
        }
        _obj = nullptr;
        _mid = nullptr;
    }
    LOG_EXIT();
}

void frame_callback::update() {
    JNIEnv *env;
    if (JNI_OK == jvm->AttachCurrentThread(&env, nullptr)){
        env->CallVoidMethod(obj, mid);
        jvm->DetachCurrentThread();
    }
}

NAMESPACE_R