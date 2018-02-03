#include "executor.hh"

Executor::Executor(const std::shared_ptr<std::recursive_mutex>& mutex,
                   const std::shared_ptr<std::condition_variable_any>& cv,
                   std::queue<std::function<void()>>& queue)
    : cv_(cv), mutex_(mutex), queue_(queue)
{
    state = State::INITED;
}

void Executor::start()
{
    if (state == State::STOPPED)
        return;

    my_thread_ = std::thread(&Executor::run, this);
}

void Executor::run()
{
    if (state == State::STOPPED)
        return;

    state = State::RUNNING;
    while (state == State::RUNNING)
    {
        std::unique_lock<std::recursive_mutex> lock(*mutex_);
        cv_->wait(lock, &Executor::need_to_run);
        std::function<void ()> f = queue_.front();
        queue_.pop();
        lock.unlock();

        f();
    }
}

bool Executor::need_to_run()
{
    return queue_.size() != 0;
}

void Executor::stop()
{
    state = State::STOPPED;
}
