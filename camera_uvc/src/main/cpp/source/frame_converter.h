//
// Created by Hsj on 2024/04/03.
//

#pragma once

#include "frame_queue.h"
#include "frame_viewer.h"
#include "frame_decoder.h"
#include "frame_encoder.h"
#include "frame_callback.h"

NAMESPACE_L

    typedef enum {
        UVC_FORMAT_ANY       = 0x00,
        UVC_FORMAT_YUY2      = 0x04,
        UVC_FORMAT_MJPG      = 0x06,
        UVC_FORMAT_H264      = 0x10,
    } uvc_format;

    typedef enum {
        FRAME_FORMAT_ORIGIN  = 0x0,
        FRAME_FORMAT_I420    = 0xE,
        FRAME_FORMAT_RGB     = 0xF,
    } frame_format;

    typedef enum {
        FRAME_ROTATE_NONE   = 0x00,
        FRAME_ROTATE_90     = 0x5A,
        FRAME_ROTATE_180    = 0xB4,
        FRAME_ROTATE_270    = 0x10E,
    } frame_rotate;

    typedef enum {
        FRAME_FLIP_NONE     = 0x0,
        FRAME_FLIP_H        =-0x1,
        FRAME_FLIP_V        = 0x1,
    } frame_flip;

    typedef struct {
        // support index
        uint8_t id;
        // support uvc format
        uint8_t fmt;
        // support uvc format name
        uint8_t name[8];
        // support width
        uint16_t width;
        // support height
        uint16_t height;
        // support fps
        uint8_t fps;

        // bandwidth ∈ (0.10f, 1.00f] for usb transfer
        float bandwidth;
        // frame format
        frame_format format;
        // frame rotate
        frame_rotate rotate;
        // frame flip
        frame_flip flip;
        // video format
        video_format video_fmt;
        // video path
        uint8_t video_dir[128];
    } support_info;

    typedef int (frame_decoder::*decoder)(uint8_t *src, size_t size, uint8_t *dst);

    typedef int (frame_encoder::*encoder)(uint8_t *src, size_t size, uint8_t *dst);

    class frame_converter {
    public:
        frame_converter(std::shared_ptr<support_info> &info);
        ~frame_converter();
        int start();
        int enqueue(void *data, size_t size);
        frame_buffer *get_buffer() const { return buffer; }
        int set_frame_callback(frame_callback *cb, void *user_ptr);
        int set_picture_callback(frame_callback *cb, void *user_ptr);
        int start_video();
        int pause_video();
        int stop_video();
        int stop();
        int destroy();

    private:
        // decoder
        decoder _decode;
        frame_decoder *_decoder;
        // encoder
        encoder _encode;
        frame_encoder *_encoder;
        // callback
        void *frame_ptr;
        void *picture_ptr;
        frame_callback *frame_cb;
        // converter
        frame_buffer *buffer;
        std::shared_ptr<frame_queue> queue;
        std::shared_ptr<support_info> info;
        std::shared_ptr<frame_decoder> decoder;
        std::shared_ptr<frame_encoder> encoder;
    };

NAMESPACE_R