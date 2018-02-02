#pragma once

#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>
#include <functional>

#include "executor.hh"

#define DEFAULT_MAX_THREADS 10

class ThreadPool
{
    public:
        ThreadPool(unsigned nb_max_threads = DEFAULT_MAX_THREADS);
        void add_task(std::function<void ()>);
        void remove_all_tasks();
        void init(bool start = true);
        void destroy();
        void start();

    private:
        std::recursive_mutex mutex_;
        std::condition_variable cv_;
        std::list<std::function<void ()>> tasks_;
        std::vector<Executor> threads_;
};
