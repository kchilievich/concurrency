#include "thread_pool/thread_pool.hpp"
#include <iostream>

int main()
{
    tp::ThreadPool thread_pool{4};

    thread_pool.Start();

    thread_pool.Submit([](){
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "Third" << std::endl;
        
        tp::ThreadPool::Current()->Submit([](){
            std::cout << "Fourth" << std::endl;
        });
    });

    thread_pool.Submit([](){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        std::cout << "Second" << std::endl;
    });

    thread_pool.Submit([](){
        std::cout << "First" << std::endl;
    });

    thread_pool.WaitIdle();
    thread_pool.Stop();

    return 0;
}