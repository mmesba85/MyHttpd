#include "executor.hh"

Executor::Executor(const std::shared_ptr<std::recursive_mutex>& mutex,
                   const std::shared_ptr<std::condition_variable>& cv,
                   std::queue<std::function<void()>>& queue)
    : cv_(cv), mutex_(mutex), queue_(queue)
{}

void Executor::start()
{
    my_thread_ = std::thread(&Executor::run, this);
}

void Executor::run()
{

}
