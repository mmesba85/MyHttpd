#pragma once

#include <thread>

class Executor
{
    public:
        void start();
    private:
        std::thread my_thread_;

};
