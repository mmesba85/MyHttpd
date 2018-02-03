#pragma once

#include <thread>
#include <condition_variable>
#include <memory>
#include <mutex>

class Executor
{
    public:
        Executor(const std::shared_ptr<std::recursive_mutex>& mutex,
                 const std::shared_ptr<std::condition_variable>& cv);
        void start();
        void stop();
    private:
        void run();
        bool need_to_wait();

        const std::shared_ptr<std::condition_variable> cv_;
        const std::shared_ptr<std::recursive_mutex> mutex_;
        std::shared_ptr<std::thread> my_thread_;
};
