//
// Created by Hsj on 2024/04/03.
//

#pragma once

#ifndef CAMERA_UVC_COMMON_HPP
#define CAMERA_UVC_COMMON_HPP

#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>
#include <android/log.h>

#define STATE_SUCCESS          0x0
#define STATE_FAILURE         -0x1
#define STATE_ERROR_MEMORY    -0x2
#define STATE_ERROR_ACCESS    -0x3
#define STATE_NOT_SUPPORT     -0x4
#define STATE_NONE_DEVICE     -0x5
#define STATE_ERROR_STATE     -0x6
#define STATE_ERROR_PARAM     -0x7
#define STATE_NONE_INIT       -0x8
#define STATE_ERROR_UNKNOWN   -0x9

#define NAMESPACE_CAMERA namespace camera
#define NAMESPACE_L NAMESPACE_CAMERA {
#define NAMESPACE_R }

#define SAFE_CLEAR(x) {memset (&(x), 0, sizeof (x));}
#define SAFE_FREE(x) {if(x != NULL){free(x); x = NULL;}}
#define SAFE_DELETE(p) {if(p) { delete (p); (p) = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p) { delete[](p); (p) = NULL;}}
#define DL_FOREACH(head,el) {for(el=head; el; el=el->next)}

#ifdef __GNUC__
    #define LIKELY(X) __builtin_expect(!!(X), 1)
    #define UNLIKELY(X) __builtin_expect(!!(X), 0)
    #define CONDITION(cond) __builtin_expect(cond!=0, 0)
#else
    #define LIKELY(X) (X)
    #define UNLIKELY(X) (X)
    #define	CONDITION(cond)	(cond)
#endif

#define LOG_TAG "camera_uvc"

#ifdef LOG_E
    #define LOGE(FMT,...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "[%d*%s:%d:%s]:" FMT,\
                            gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOGE(...)
#endif

#ifdef LOG_W
    #define LOGW(FMT,...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "[%d*%s:%d:%s]:" FMT, \
                            gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOGW(...)
#endif

#ifdef LOG_I
    #define LOGI(FMT,...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[%d*%s:%d:%s]:" FMT, \
                            gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOGI(...)
#endif

#ifdef LOG_D
    #define LOGD(FMT,...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%d*%s:%d:%s]:" FMT,\
                            gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOGD(...)
#endif

#define LOG_INTO() LOGD(" -> into.")
#define LOG_EXIT() LOGD(" -> exit.")

#endif //CAMERA_UVC_COMMON_HPP