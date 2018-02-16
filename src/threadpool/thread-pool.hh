#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <memory>
#include <exception>

#include "executor.hh"

constexpr unsigned DEFAULT_MAX_THREADS = 10;

class ThreadPool
{
    enum class State
    {
        STARTED,
        STOPPED,
        INITED
    };

    public:
        ThreadPool();
        ThreadPool(unsigned nb_max_threads, bool start_now);
        ~ThreadPool();
        void add_task(const std::function<void ()>);
        void remove_all_tasks();
        void destroy();
        void start();
        void sync();
    private:
        std::recursive_mutex mutex_;
        std::condition_variable_any cv_;
        std::queue<std::function<void ()>> tasks_;
        std::vector<std::shared_ptr<Executor>> threads_;

        enum State state;
};
