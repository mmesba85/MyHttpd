#include "executor.hh"

Executor::Executor(std::recursive_mutex& mutex,
        std::condition_variable_any& cv,
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
    
    state = State::WAITING;
    while (state == State::WAITING)
    {
        mutex_.lock();
        try{
            while (!need_to_run() && state != State::STOPPED)
                cv_.wait(mutex_);
            if (state == State::STOPPED)
            {
                mutex_.unlock();
                return;
            }
            state = State::RUNNING;
            std::function<void ()> f = queue_.front();
            queue_.pop();
            mutex_.unlock();
                
            f();
            state = State::WAITING;
        }
        catch(const std::exception&)
        {
            mutex_.unlock();
        }
    }
}

bool Executor::need_to_run()
{
    return queue_.size();
}

void Executor::stop()
{
    state = State::STOPPED;
    my_thread_.detach();
}

bool Executor::is_waiting()
{
    return state == State::WAITING;
}
