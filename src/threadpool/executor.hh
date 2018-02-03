#pragma once

#include <thread>
#include <condition_variable>
#include <memory>
#include <queue>
#include <functional>
#include <mutex>

class Executor
{
    enum class State
    {
        RUNNING,
        STOPPED,
        INITED
    };

    public:
        Executor(const std::shared_ptr<std::recursive_mutex>& mutex,
                 const std::shared_ptr<std::condition_variable_any>& cv,
                 std::queue<std::function<void()>>& queue);
        void start();
        void stop();
    private:
        void run();
        bool need_to_run();

        const std::shared_ptr<std::condition_variable_any> cv_;
        const std::shared_ptr<std::recursive_mutex> mutex_;
        std::queue<std::function<void ()>>& queue_;
        std::thread my_thread_;

        enum State state;
};
