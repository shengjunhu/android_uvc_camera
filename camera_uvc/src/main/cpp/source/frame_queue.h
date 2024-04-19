//
// Created by Hsj on 2024/04/03.
//

#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include "common.hpp"

NAMESPACE_L

    typedef struct
    {
        uint8_t *data;
        size_t size;
    } frame;

    class frame_queue
    {
    public:
        explicit frame_queue(size_t frame_max_size);
        ~frame_queue();
        bool start(std::function<void(frame)> cb);
        bool enqueue(void *data, size_t size);
        bool stop();
    private:
        std::function<void(frame)> dequeue;
        std::queue<frame> producer_queue;
        std::queue<frame> consumer_queue;
        std::condition_variable cv;
        std::thread thread_queue;
        std::mutex mtx;
        bool active;

        #define QUEUE_SIZE 5
        frame frames[QUEUE_SIZE];
        uint8_t *buffers;
    };

NAMESPACE_R