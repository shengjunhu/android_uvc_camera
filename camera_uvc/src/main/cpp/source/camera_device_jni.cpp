//
// Created by Hsj on 2024/04/03.
//

#include <jni.h>
#include <android/native_window_jni.h>
#include "camera_device.h"

#define HANDLE "handle"
#define CAMERA_DEVICE "android/camera/uvc/CameraDevice"

static JavaVM *jvm;
typedef jlong CAMERA_ID;
using NAMESPACE_CAMERA;

static void set_field_long(JNIEnv *env, jobject obj, const char *name, jlong value) {
    jclass clazz = env->GetObjectClass(obj);
    jfieldID field = env->GetFieldID(clazz, name, "J");
    if (LIKELY(field)) {
        env->SetLongField(obj, field, value);
    } else {
        LOGE("set_field_long: GetFieldID(%s).", name);
    }
    env->DeleteLocalRef(clazz);
}

static CAMERA_ID create(JNIEnv *env, jobject thiz) {
    auto *camera = new camera_uvc();
    auto id = reinterpret_cast<CAMERA_ID>(camera);
    set_field_long(env, thiz, HANDLE, id);
    return id;
}

static jint open_device(JNIEnv *env, jclass clazz, CAMERA_ID id, jstring fs, jint fd) {
    jint ret = STATE_NONE_INIT;
    if (!fs || fd < 1) {
        LOGE("Invalid parameter.");
    } else {
        auto *camera = reinterpret_cast<camera_uvc *>(id);
        if (camera) {
            const char *_fs = env->GetStringUTFChars(fs, JNI_FALSE);
            ret = camera->open_device(_fs, fd);
            env->ReleaseStringUTFChars(fs, _fs);
        } else {
            LOGE("camera already destroyed.");
        }
    }
    return ret;
}

static jint get_support(JNIEnv *env, jobject thiz, CAMERA_ID id, jobject list) {
    jint ret = STATE_NONE_INIT;
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        if (width > 0 && height > 0) {
            ret = camera->set_frame_size(width, height);
            if (ret == STATE_SUCCESS) {
                jclass cls = env->GetObjectClass(thiz);
                jfieldID fId = env->GetFieldID(cls, "frame", "Ljava/nio/ByteBuffer;");
                size_t buffer_size = camera->get_buffer_size();
                if (buffer_size > 0) {
                    jobject bb = env->NewDirectByteBuffer(camera->get_buffer(), buffer_size);
                    env->SetObjectField(thiz, fId, bb);
                    jmethodID mid = env->GetMethodID(cls, "updateFrame", "()V");
                    jobject obj = env->NewGlobalRef(thiz);
                    camera->set_callback(jvm, obj, mid);
                    ret = STATE_SUCCESS;
                } else {
                    ret = STATE_FAILURE;
                }
            }
        } else {
            ret = STATE_INVALID_ARG;
            LOGE("Invalid parameter.");
        }
    } else {
        LOGW("camera already destroyed.");
    }
    return ret;
}

static jint set_support(JNIEnv *env, jobject thiz, CAMERA_ID id, jobject support) {
    if (!support) {
        LOGW("Invalid parameter.");
        return STATE_ERROR_PARAM;
    } else {
        auto *camera = reinterpret_cast<camera_uvc *>(id);
        if (camera) {
            jclass cls = env->GetObjectClass(support);
            jfieldID fid1 = env->GetFieldID(cls, "id","I");
            jfieldID fid2 = env->GetFieldID(cls, "fmt", "I");
            jfieldID fid3 = env->GetFieldID(cls, "name","I");
            jfieldID fid4 = env->GetFieldID(cls, "width", "I");
            jfieldID fid5 = env->GetFieldID(cls, "height", "I");
            jfieldID fid6 = env->GetFieldID(cls, "fps","I");
            jfieldID fid7 = env->GetFieldID(cls, "bandwidth", "I");
            jfieldID fid8 = env->GetFieldID(cls, "format","I");
            jfieldID fid9 = env->GetFieldID(cls, "rotate",   "I");
            jfieldID fid10 = env->GetFieldID(cls, "flip", "I");
            jfieldID fid11 = env->GetFieldID(cls, "video", "I");
            jfieldID fid12 = env->GetFieldID(cls, "dir", "I");
            support_info info(
                    env->GetIntField(support, fid1),
                    env->GetIntField(support, fid2),
                    env->GetIntField(support, fid3),
                    env->GetIntField(support, fid4),
                    env->GetIntField(support, fid5),
                    env->GetIntField(support, fid6),
                    env->GetIntField(support, fid7),
                    env->GetIntField(support, fid8),
                    env->GetIntField(support, fid9),
                    env->GetIntField(support, fid10),
                    env->GetIntField(support, fid11),
                    env->GetIntField(support, fid12));
            return camera->set_support(info);
        } else {
            LOGW("camera already destroyed.");
            return STATE_NONE_INIT;
        }
    }

    jint ret = STATE_NONE_INIT;
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        if (width > 0 && height > 0) {
            ret = camera->set_frame_size(width, height);
            if (ret == STATE_SUCCESS) {
                jclass cls = env->GetObjectClass(thiz);
                jfieldID fId = env->GetFieldID(cls, "frame", "Ljava/nio/ByteBuffer;");
                size_t buffer_size = camera->get_buffer_size();
                if (buffer_size > 0) {
                    jobject bb = env->NewDirectByteBuffer(camera->get_buffer(), buffer_size);
                    env->SetObjectField(thiz, fId, bb);
                    jmethodID mid = env->GetMethodID(cls, "updateFrame", "()V");
                    jobject obj = env->NewGlobalRef(thiz);
                    camera->set_callback(jvm, obj, mid);
                    ret = STATE_SUCCESS;
                } else {
                    ret = STATE_FAILURE;
                }
            }
        } else {
            ret = STATE_INVALID_ARG;
            LOGE("Invalid parameter.");
        }
    } else {
        LOGW("camera already destroyed.");
    }
    return ret;
}

static jint set_preview(JNIEnv *env, jclass clazz, CAMERA_ID id, jobject surface) {
    if (!surface) {
        LOGW("Invalid parameter.");
        return STATE_ERROR_PARAM;
    } else {
        auto *camera = reinterpret_cast<camera_uvc *>(id);
        if (camera) {
            return camera->set_preview(ANativeWindow_fromSurface(env, surface));
        } else {
            LOGW("camera already destroyed.");
            return STATE_NONE_INIT;
        }
    }
}

static jint start_stream(JNIEnv *env, jclass clazz, CAMERA_ID id) {
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera != nullptr) {
        return camera->start_stream();
    } else {
        LOGW("camera already destroyed.");
        return STATE_NONE_INIT;
    }
}

static jint take_picture(JNIEnv *env, jclass clazz, CAMERA_ID id) {
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        return camera->take_picture();
    } else {
        LOGW("camera already destroyed.");
        return STATE_NONE_INIT;
    }
}

static jint start_video(JNIEnv *env, jclass clazz, CAMERA_ID id) {
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        return camera->start_video();
    } else {
        LOGW("camera already destroyed.");
        return STATE_NONE_INIT;
    }
}

static jint pause_video(JNIEnv *env, jclass clazz, CAMERA_ID id, jboolean pause) {
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        return camera->pause_video(pause);
    } else {
        LOGW("camera already destroyed.");
        return STATE_NONE_INIT;
    }
}

static jint stop_video(JNIEnv *env, jclass clazz, CAMERA_ID id) {
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        return camera->stop_video();
    } else {
        LOGW("camera already destroyed.");
        return STATE_NONE_INIT;
    }
}

static jint stop_stream(JNIEnv *env, jclass clazz, CAMERA_ID id) {
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        return camera->stop_stream();
    } else {
        LOGW("camera already destroyed.");
        return STATE_NONE_INIT;
    }
}

static jint close_device(JNIEnv *env, jclass clazz, CAMERA_ID id) {
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        return camera->close_device();
    } else {
        LOGW("camera already destroyed.");
        return STATE_NONE_INIT;
    }
}

static void destroy(JNIEnv *env, jobject thiz, CAMERA_ID id) {
    auto *camera = reinterpret_cast<camera_uvc *>(id);
    if (camera) {
        set_field_long(env, thiz, HANDLE, 0);
        camera->destroy();
        SAFE_DELETE(camera);
    } else {
        LOGW("camera already destroyed.");
    }
}

//==================================================================================================

static const JNINativeMethod CAMERA_DEVICE_METHODS[] = {
    {"create",       "()J",                                  (void *) create},
    {"open_device",  "(JLjava/lang/String;I)I",              (void *) open_device},
    {"get_support",  "(JLjava/util/List;)I",                 (void *) get_support},
    {"set_support",  "(JLandroid/camera/uvc/SupportInfo;)I", (void *) set_support},
    {"set_preview",  "(JLandroid/view/Surface;)I",           (void *) set_preview},
    {"start_stream", "(J)I",                                 (void *) start_stream},
    {"take_picture", "(J)I",                                 (void *) take_picture},
    {"start_video",  "(J)I",                                 (void *) start_video},
    {"pause_video",  "(JZ)I",                                (void *) pause_video},
    {"stop_video",   "(J)I",                                 (void *) stop_video},
    {"stop_stream",  "(J)I",                                 (void *) stop_stream},
    {"close_device", "(J)I",                                 (void *) close_device},
    {"destroy",      "(J)",                                  (void *) destroy},
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void* reserved) {
    JNIEnv *env;
    jint ret = vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (ret == JNI_OK) {
        jclass cls = env->FindClass(CAMERA_DEVICE);
        if (cls) {
            jint size = sizeof(CAMERA_DEVICE_METHODS) / sizeof(JNINativeMethod);
            ret = env->RegisterNatives(cls, CAMERA_DEVICE_METHODS, size);
            if (ret == JNI_OK) jvm = vm;
        } else {
            LOGE("JNI_OnLoad: FindClass(%s).", CAMERA_DEVICE);
            ret = JNI_ERR;
        }
    } else {
        LOGE("JNI_OnLoad: GetEnv() %d.", ret);
    }
    return ret == JNI_OK ? JNI_VERSION_1_6 : ret;
}

//==================================================================================================