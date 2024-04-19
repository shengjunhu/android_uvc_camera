//
// Created by Hsj on 2024/04/03.
//

#include <thread>
#include <fcntl.h>

#include "camera_device.h"
#include "libuvc/libuvc_internal.h"

NAMESPACE_L

camera_sys::camera_sys() :
_fd(0),
_info(nullptr),
_device(nullptr),
_converter(nullptr),
_device_handle(nullptr)
{
    // 1.1 uvc init.
    uvc_error_t uvc_ret = uvc_init(&_context, nullptr);
    if (uvc_ret == uvc_error_t::UVC_SUCCESS) {
        set_state(camera_state::INIT);
    } else {
        set_state(camera_state::ERROR);
        LOGE("uvc_init() failed: %d.", uvc_ret);
    }
}

camera_sys::~camera_sys()
{
    // 9.1 uvc exit.
    if (_context != nullptr) {
        uvc_exit(_context);
        _context = nullptr;
    }
}

int su_chmod(const char *path)
{
    if (path == nullptr || strlen(path) > 100){
        LOGE("path(%s) is invalid.", path);
        return STATE_INVALID_ARG;
    }
    char cmd[100]; sprintf(cmd, "su -c \"chmod 666 %s\"", path);
    int ret = system(cmd);
    if (STATE_FAILURE == ret) {
        LOGE("system(%s) failed: %d.", cmd, ret);
        return STATE_FAILURE;
    } else if (0 == WIFEXITED(ret)) {
        LOGE("exit cmd failed.");
        return STATE_FAILURE;
    } else if (0 != WEXITSTATUS(ret)) {
        LOGE("exit status failed.");
        return STATE_FAILURE;
    } else {
        return STATE_SUCCESS;
    }
}

int camera_sys::open_device(const int fd)
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 2.1 check parameter.
    if (fd <= 0) {
        LOGE("fd(%d) is invalid.", fd);
        return STATE_INVALID_ARG;
    }
    // 2.2 check camera state.
    camera_state state = get_state();
    if (state != camera_state::INIT){
        LOGE("camera state(%d) error.", state);
        return STATE_ERROR_STATE;
    }
    // 2.3 uvc wrap.
    _fd = dup(fd);
    uvc_error_t uvc_ret = uvc_wrap(_fd, _context, &_device_handle);
    if (uvc_ret == uvc_error_t::UVC_SUCCESS) {
        set_state(camera_state::OPENED);
        return STATE_SUCCESS;
    } else {
        LOGE("uvc_wrap() failed: %d.", uvc_ret);
        close(_fd); _fd = 0;
        return STATE_FAILURE;
    }
}

int camera_sys::open_device(const char *fs)
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 2.1 check parameter.
    if (fs == nullptr) {
        LOGE("fs is empty.");
        return STATE_INVALID_ARG;
    } else if (STATE_SUCCESS != strncmp(fs, "/dev/bus/usb/", 13)) {
        LOGE("fs(%s) is invalid.", fs);
        return STATE_INVALID_ARG;
    }
    // 2.2 check camera state.
    camera_state state = get_state();
    if (state != camera_state::INIT) {
        LOGE("camera state(%d) error.", state);
        return STATE_ERROR_STATE;
    }
    // 2.3 check access fs permission and grant permission.
    if (STATE_SUCCESS != access(fs, W_OK)) su_chmod(fs);
    // 2.4 open fs, if had access permission.
    int ret = STATE_ERROR_ACCESS;
    if (STATE_SUCCESS == access(fs, W_OK)) {
        int fd = open(fs, O_RDWR | O_NONBLOCK, S_IRWXU);
        if (fd > 0) {
            ret = open_device(fd);
            if (STATE_SUCCESS != ret) close(fd);
        } else {
            ret = STATE_FAILURE;
            LOGE("fs(%s) open failed: %d.", fs, fd);
        }
    } else {
        LOGE("fs(%s) can't be open.", fs);
    }
    return ret;
}

int camera_sys::open_device(const int vid, const int pid)
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 2.1 check parameter.
    if (vid <= 0 || pid <= 0) {
        LOGE("vid(%d) and pid(%d) is invalid.", vid, pid);
        return STATE_INVALID_ARG;
    }
    // 2.2 check camera state.
    camera_state state = get_state();
    if (state != camera_state::INIT){
        LOGE("camera state(%d) error.", state);
        return STATE_ERROR_STATE;
    }
    // 2.3 find the first matching device. uvc_find_devices()
    uvc_error_t uvc_ret = uvc_find_device(_context, &_device, vid, pid, nullptr);
    if (uvc_ret != uvc_error_t::UVC_SUCCESS || _device == nullptr) {
        LOGE("no device by vid:%d_pid:%d.", vid, pid);
        return STATE_NO_DEVICE;
    }
    // 2.4 check access fs permission and grant permission.
    int bus = libusb_get_bus_number(_device->usb_dev);
    int dev = libusb_get_device_address(_device->usb_dev);
    char fs[100]; sprintf(fs, "/dev/bus/usb/%d/%d", bus, dev);
    if (STATE_SUCCESS != access(fs, W_OK)) su_chmod(fs);
    // 2.5  open device, if had access permission.
    int ret = STATE_ERROR_ACCESS;
    if (STATE_SUCCESS == access(fs, W_OK)) {
#ifdef USE_FD
        int fd = open(fs, O_RDWR | O_NONBLOCK, S_IRWXU);
        if (fd > 0) {
            ret = open_device(fd);
            if (STATE_SUCCESS != ret) close(fd);
        } else {
            ret = STATE_FAILURE;
            LOGE("fs(%s) open failed: %d.", fs, fd);
        }
#else
        uvc_ret = uvc_open(_device, &_device_handle);
        if (uvc_ret == uvc_error_t::UVC_SUCCESS) {
            set_state(camera_state::OPENED);
            ret = STATE_SUCCESS;
        } else {
            LOGE("uvc_open() failed: %d", uvc_ret);
            ret = STATE_FAILURE;
        }
#endif
    } else {
        LOGE("fs(%s) can't be open.", fs);
    }
    return ret;
}

int camera_sys::get_support(std::vector<support_info> &list)
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 3.1 check camera state.
    camera_state state = get_state();
    if (state < camera_state::OPENED){
        LOGE("camera state(%d) error.", state);
        return STATE_ERROR_STATE;
    }
    // 3.2 check device handle.
    if (_device_handle == nullptr) {
        LOGE("_device_handle is empty.");
        return STATE_FAILURE;
    }
    // 3.3 get device info.
    uvc_device_info *device_info = _device_handle->info;
    if (device_info == nullptr) {
        LOGE("device_info is empty.");
        return STATE_FAILURE;
    }
    uvc_streaming_interface_t *stream_ifs = device_info->stream_ifs;
    if (stream_ifs == nullptr) {
        LOGE("stream_ifs is empty.");
        return STATE_FAILURE;
    }
    uvc_streaming_interface_t *stream_if;
    DL_FOREACH(stream_ifs, stream_if) {
        uvc_format_desc_t *fmt_desc;
        uvc_frame_desc_t *frame_desc;
        DL_FOREACH(stream_if->format_descs, fmt_desc) {
            DL_FOREACH(fmt_desc->frame_descs, frame_desc) {
                support_info info;
                info.id = fmt_desc->bDescriptorSubtype;
                info.width     = frame_desc->wWidth;
                info.height    = frame_desc->wHeight;
                info.fps       = 10000000 / frame_desc->dwDefaultFrameInterval;
                memcpy(info.name, fmt_desc->fourccFormat, 4);
                list.emplace_back(info);
            }
        }
    }
    return STATE_SUCCESS;
}

int camera_sys::set_support(support_info &info)
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 4.1 check parameter.
    if (info.bandwidth <= 0.0 || info.bandwidth > 1.0) {
        LOGE("info.bandwidth(%f) is invalid.", info.bandwidth);
        return STATE_INVALID_ARG;
    }
    // 4.2 check camera state.
    camera_state state = get_state();
    if (state != camera_state::OPENED && state != camera_state::CONFIGURED){
        LOGE("camera state(%d) error.", state);
        return STATE_ERROR_STATE;
    }
    // 4.3 set stream param
    if (_device_handle == nullptr) {
        LOGE("_device_handle is empty.");
        return STATE_FAILURE;
    }
    enum uvc_frame_format format;
    if (info.id == uvc_vs_desc_subtype::UVC_VS_FORMAT_UNCOMPRESSED){
        format = uvc_frame_format::UVC_FRAME_FORMAT_YUYV;
    } else if (info.id == uvc_vs_desc_subtype::UVC_VS_FORMAT_MJPEG) {
        format = uvc_frame_format::UVC_FRAME_FORMAT_MJPEG;
    } else if (info.id == uvc_vs_desc_subtype::UVC_VS_FORMAT_FRAME_BASED) {
        format = uvc_frame_format::UVC_FRAME_FORMAT_H264;
    } else {
        format = uvc_frame_format::UVC_FRAME_FORMAT_ANY;
    }
    uvc_error_t uvc_ret = uvc_get_stream_ctrl_format_size(_device_handle,
        &_stream_ctrl, format, info.width, info.height, info.fps);
    if (uvc_ret == uvc_error_t::UVC_SUCCESS) {
        // 4.4 create frame_converter.
        _info = std::make_shared<support_info>();
        memcpy(_info.get(), &info, sizeof(support_info));
        _converter = std::make_shared<frame_converter>(_info);
        info.pxl_length = _converter->get_buffer_size();
        set_state(camera_state::CONFIGURED);
        return STATE_SUCCESS;
    } else {
        LOGE("uvc_get_stream_ctrl_format_size() failed: %d.", uvc_ret);
        return STATE_FAILURE;
    }
}

static void get_frame_callback(uvc_frame_t *frame, void *args)
{
    auto *converter = reinterpret_cast<frame_converter *>(args);
    converter->enqueue(frame->data, frame->data_bytes);
}

int camera_sys::start_stream(frame_callback *cb, void *user_ptr)
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 5.1 check camera state.
    camera_state state = get_state();
    if (state == camera_state::STREAMING){
        return STATE_SUCCESS;
    } else if (state != camera_state::CONFIGURED) {
        LOGE("camera state(%d) error.", state);
        return STATE_ERROR_STATE;
    }
    // 5.2 set frame callback
    _converter->set_frame_callback(cb, user_ptr);
    _converter->start();
    // 5.3 start streaming
    uvc_error_t uvc_ret = uvc_start_streaming(_device_handle, &_stream_ctrl,
        get_frame_callback, _converter.get(), _info->bandwidth);
    if (uvc_ret == uvc_error_t::UVC_SUCCESS) {
        set_state(camera_state::STREAMING);
        return STATE_SUCCESS;
    } else {
        LOGE("uvc_start_streaming() failed: %d.", uvc_ret);
        return STATE_FAILURE;
    }
}

int camera_sys::take_picture(frame_callback *cb, void *user_ptr)
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 6.1 check state is on streaming.
    camera_state state = get_state();
    if (state == camera_state::STREAMING) {
        return _converter->set_picture_callback(cb, user_ptr);
    } else {
        LOGE("camera state(%d) error.", state);
        return STATE_ERROR_STATE;
    }
}

int camera_sys::start_video()
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 6.2 check camera state
    int ret = STATE_ERROR_STATE;
    camera_state state = get_state();
    if (state >= camera_state::STREAMING) {
        ret = _converter->start_video();
        if (ret == STATE_SUCCESS) {
            set_state(camera_state::START_VIDEO);
        }
    } else {
        LOGE("camera state(%d) error.", state);
    }
    return ret;
}

int camera_sys::pause_video()
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 6.3 check camera state
    camera_state state = get_state();
    if (state == camera_state::START_VIDEO) {
        _converter->pause_video();
        set_state(camera_state::PAUSE_VIDEO);
    } else {
        LOGW("camera state(%d) error.", state);
    }
    return STATE_SUCCESS;
}

int camera_sys::stop_video()
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 6.4 check camera state
    camera_state state = get_state();
    if (state > camera_state::STREAMING) {
        _converter->stop_video();
        set_state(camera_state::STREAMING);
    } else {
        LOGW("camera state(%d) error.", state);
    }
    return STATE_SUCCESS;
}

int camera_sys::stop_stream()
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 7.1 stop record video.
    camera_state state = get_state();
    if (state > camera_state::STREAMING) stop_video();
    // 7.2 stop streaming.
    if (state == camera_state::STREAMING) {
        uvc_stop_streaming(_device_handle);
        set_state(camera_state::CONFIGURED);
    }
    // 7.3 converter stop.
    _converter->stop();
    return STATE_SUCCESS;
}

int camera_sys::close_device()
{
    std::lock_guard<std::mutex> lk(_mutex);
    // 8.1 check stream state.
    camera_state state = get_state();
    if (state >= camera_state::STREAMING) stop_stream();
    // 8.2 close device handle.
    if (_device_handle != nullptr) {
        uvc_close(_device_handle);
        _device_handle = nullptr;
    }
    // 8.3 close device.
    if (_fd > 0) {
        close(_fd); _fd = 0;
    } else if (_device != nullptr){
        uvc_unref_device(_device);
        _device = nullptr;
    }
    set_state(camera_state::INIT);
    return STATE_SUCCESS;
}

int camera_sys::destroy()
{
    _info.reset();
    _converter.reset();
    set_state(camera_state::ERROR);
    // 9.1 uvc exit.
    if (_context != nullptr) {
        uvc_exit(_context);
        _context = nullptr;
    }
    return STATE_SUCCESS;
}

NAMESPACE_R