//
// Created by Hsj on 2024/04/03.
//

#include "frame_queue.h"

NAMESPACE_L

frame_queue::frame_queue(size_t frame_max_size) : active(false)
{
    int i = 0;
    buffers = new uint8_t[frame_max_size * QUEUE_SIZE];
    do {
        frames[i].start = buffers + frame_max_size * i;
        producer_queue.push(frames[i]);
    } while (++i < QUEUE_SIZE);
}

frame_queue::~frame_queue()
{
    active = false;
    while (!consumer_queue.empty()) consumer_queue.pop();
    while (!producer_queue.empty()) producer_queue.pop();
    SAFE_DELETE_ARRAY(buffers);
    LOGI("~frame_queue()");
}

bool frame_queue::enqueue(void *data, size_t size)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (!active) return false;
    frame f;
    if (!producer_queue.empty()) {
        f = producer_queue.front();
        producer_queue.pop();
    } else {
        f = consumer_queue.front();
        consumer_queue.pop();
    }
    lock.unlock();
    // copy frame
    memcpy(f.start, data, size);
    f.length = size;
    // push to consumer queue
    lock.lock();
    consumer_queue.push(f);
    lock.unlock();
    // notify consumer queue
    cv.notify_one();
    return true;
}

bool frame_queue::start(std::function<void(frame)> cb)
{
    if (!active) {
        active = true;
        dequeue = std::move(cb);
        thread_queue = std::thread([&]() {
            while (active) {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]() { return (active != consumer_queue.empty()); });
                if (consumer_queue.empty()) continue;
                frame f  = consumer_queue.front();
                consumer_queue.pop();
                lock.unlock();
                // callback frame to decode
                dequeue(f);
                // put buffer to producer queue
                lock.lock();
                producer_queue.push(f);
                lock.unlock();
            }
        });
    }
    return true;
}

bool frame_queue::stop()
{
    active = false;
    cv.notify_one();
    if (thread_queue.joinable()) thread_queue.join();
    std::unique_lock<std::mutex> lock(mtx);
    while (!consumer_queue.empty()) {
        frame f  = consumer_queue.front();
        consumer_queue.pop();
        producer_queue.push(f);
    }
    return true;
}

NAMESPACE_R