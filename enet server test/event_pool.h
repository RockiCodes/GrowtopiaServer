#pragma once

#include <queue>
#include <chrono>
#include <thread>
#include <functional>

class EventPool {
public:
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

    struct Event {
        TimePoint time;
        std::function<void()> action;

        bool operator<(const Event& other) const {
            return time > other.time;
        }
    };

    void schedule(const Event& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.push(event);
        if (events_.top().time == event.time) {
            cv_.notify_all();
        }
    }

    void run() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            while (events_.empty() || events_.top().time > std::chrono::system_clock::now()) {
                if (!events_.empty()) {
                    cv_.wait_until(lock, events_.top().time);
                }
                else {
                    cv_.wait(lock);
                }
            }
            auto event = events_.top();
            events_.pop();
            lock.unlock();
            event.action();
        }
    }

private:
    std::priority_queue<Event> events_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

/*int main() {
    EventPool eventPool;

    // schedule a function to run after 5 seconds
    auto now = std::chrono::system_clock::now();
    auto fiveSecondsLater = now + std::chrono::seconds(5);
    eventPool.schedule(EventPool::Event{ fiveSecondsLater, []() { std::cout << "Hello, world!" << std::endl; } });
    std::thread eventThread(&EventPool::run, &eventPool);
    eventThread.join();
}*/