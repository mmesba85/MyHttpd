#pragma once

#include <thread>
#include <condition_variable>
#include <memory>
#include <queue>
#include <functional>
#include <mutex>

class Executor
{
    public:
        Executor(const std::shared_ptr<std::recursive_mutex>& mutex,
                 const std::shared_ptr<std::condition_variable>& cv,
                 std::queue<std::function<void()>>& queue);
        void start();
        void stop();
    private:
        void run();
        bool need_to_wait();

        const std::shared_ptr<std::condition_variable> cv_;
        const std::shared_ptr<std::recursive_mutex> mutex_;
        std::queue<std::function<void ()>>& queue_;
        std::thread my_thread_;
};
