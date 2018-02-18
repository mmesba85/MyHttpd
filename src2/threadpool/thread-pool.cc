#include "thread-pool.hh"
#include <iostream>

ThreadPool::ThreadPool()
    : ThreadPool(DEFAULT_MAX_THREADS, true)
{}

ThreadPool::ThreadPool(unsigned nb_max_threads, bool start_now)
{
    if (nb_max_threads == 0)
        throw std::invalid_argument(
                "ThreadPool must be created with at least one thread");

    for(unsigned i = 0; i < nb_max_threads; ++i)
        threads_.push_back(std::make_shared<Executor>(mutex_, cv_, tasks_));

    state = State::INITED;
    if (start_now)
        start();
}

void ThreadPool::start()
{
    if (state == State::STARTED || state == State::STOPPED)
        return;

    for (auto& executor : threads_)
        executor->start();
    cv_.notify_all();
}

void ThreadPool::add_task(const std::function<void ()> function)
{
    if (state == State::STOPPED)
        return;

    std::unique_lock<std::recursive_mutex> lock(mutex_);
    tasks_.push(function);
    cv_.notify_one();
}

void ThreadPool::remove_all_tasks()
{
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    while (tasks_.size())
        tasks_.pop();
}

void ThreadPool::destroy()
{
    if (state == State::STOPPED)
        return;

    this->remove_all_tasks();

    for (std::shared_ptr<Executor>& executor : threads_)
        executor->stop();
    threads_.clear();
    state = State::STOPPED;
    cv_.notify_all();
}

ThreadPool::~ThreadPool()
{
    //std::cout << "starting destroying threadpool" << std::endl;
    sync();
    //std::cout << "after syncking and now destroying" << std::endl;
    destroy();
    //std::cout << "after destroying" << std::endl;
}

void ThreadPool::sync()
{
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    while (tasks_.size())
    {
        lock.unlock();
        std::this_thread::yield();
        lock.lock();
    }
    
    //std::cout << "tasks are empty" << std::endl;
    //size_t i = 0;
    for (auto& thread : threads_)
    {
        while (!thread->is_waiting())
        {
            thread->print();
            lock.unlock();
            std::this_thread::yield();
            lock.lock();
        }
        //++i;
        //std::cout << i << std::endl;
    }
}
