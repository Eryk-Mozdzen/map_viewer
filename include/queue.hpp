#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>

template <typename T>
class queue {
    std::queue<T> queue_;
    std::condition_variable cond_;
    mutable std::mutex mutex_;

public:
    void push(const T &item) {
        const std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(item);
        cond_.notify_one();
    }

    T pop(const std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!cond_.wait_for(lock, timeout, [this]() { return !queue_.empty(); })) {
            throw std::exception();
        }
        T item = queue_.front();
        queue_.pop();
        return item;
    }

    bool contains(const T &value) const {
        const std::unique_lock<std::mutex> lock(mutex_);
        std::queue<T> copy = queue_;
        while(!copy.empty()) {
            if(copy.front() == value) {
                return true;
            }
            copy.pop();
        }
        return false;
    }
};

#endif
