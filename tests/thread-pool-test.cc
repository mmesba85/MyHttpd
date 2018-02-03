#include "thread-pool.hh"
#include <iostream>

int main()
{
    ThreadPool tp(2);
    for (int i = 0; i < 10; ++i)
        tp.add_task([]()
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "a " << std::this_thread::get_id() << std::endl;
        });
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "before sync" << std::endl;
    tp.sync();
    std::cout << "after sync" << std::endl;
    tp.destroy();
    return 0;
}
