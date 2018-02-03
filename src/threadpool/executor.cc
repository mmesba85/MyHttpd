#include "executor.hh"

Executor::Executor(const std::shared_ptr<std::recursive_mutex>& mutex,
                   const std::shared_ptr<std::condition_variable>& cv)
    : cv_(cv), mutex_(mutex)
{
    std::make_shared<std::thread>(start);
}

void Executor::start()
{

}
