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
        INITED,
        WAITING
    };

    public:
        Executor(std::recursive_mutex& mutex,
                 std::condition_variable_any& cv,
                 std::queue<std::function<void()>>& queue);
        void start();
        void stop();
        bool is_waiting();
    private:
        void run();
        bool need_to_run();

        std::condition_variable_any& cv_;
        std::recursive_mutex& mutex_;
        std::queue<std::function<void ()>>& queue_;
        std::thread my_thread_;

        enum State state;
};
