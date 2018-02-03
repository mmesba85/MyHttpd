#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <memory>
#include <exception>

#include "executor.hh"

#define DEFAULT_MAX_THREADS 1

class ThreadPool
{
    enum class State
    {
        STARTED,
        STOPPED,
        INITED
    };

    public:
        ThreadPool(unsigned nb_max_threads = DEFAULT_MAX_THREADS, bool start_now
                = true);
        void add_task(std::function<void ()>&);
        void remove_all_tasks();
        void destroy();
        void start();

    private:
        std::shared_ptr<std::recursive_mutex> mutex_;
        std::shared_ptr<std::condition_variable_any> cv_;
        std::queue<std::function<void ()>> tasks_;
        std::vector<std::shared_ptr<Executor>> threads_;

        enum State state;
};
