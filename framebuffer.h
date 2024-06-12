#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <queue>
#include <mutex>
#include <condition_variable>
extern "C" {
#include <libavutil/frame.h>
}
class FrameBuffer
{
public:
    FrameBuffer(size_t maxSize) : maxSize(maxSize) {}

    void push(AVFrame* frame) {
        std::unique_lock<std::mutex> lock(mutex);
        while (buffer.size() >= maxSize) {
            notFull.wait(lock);
        }
        buffer.push(av_frame_clone(frame));
        notEmpty.notify_one();
    }

    AVFrame* pop() {
        std::unique_lock<std::mutex> lock(mutex);
        while (buffer.empty()) {
            notEmpty.wait(lock);
        }
        AVFrame* frame = buffer.front();
        buffer.pop();
        notFull.notify_one();
        return frame;
    }

    AVFrame* peek() {
        std::unique_lock<std::mutex> lock(mutex);
        while (buffer.empty()) {
            notEmpty.wait(lock);
        }
        return av_frame_clone(buffer.front());
    }

private:
    std::queue<AVFrame*> buffer;
    size_t maxSize;
    std::mutex mutex;
    std::condition_variable notEmpty;
    std::condition_variable notFull;
};

#endif // FRAMEBUFFER_H
