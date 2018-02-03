#include "thread-pool.hh"

ThreadPool::ThreadPool(unsigned nb_max_threads, bool start_now)
{
    if (nb_max_threads == 0)
        throw std::invalid_argument(
                "ThreadPool must be created with at least one thread");

    for(unsigned i = 0; i < nb_max_threads; ++i)
        threads_.push_back(std::make_shared<Executor>(cv_, mutex_, tasks_));

    state = State::INITED;
    if (start_now)
        start();
}

void ThreadPool::start()
{
    if (state == State::STARTED)
        return;
    for (auto& executor : threads_)
        executor->start();
}
