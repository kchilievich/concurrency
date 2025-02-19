#include "thread_pool.hpp"
#include <iostream>

// NOTE: Temporary placement, reconsider where it must be
thread_local tp::ThreadPool* g_LocalThreadPool;

tp::ThreadPool::ThreadPool(size_t threads)
{
    threads_.reserve(threads);
}

tp::ThreadPool::~ThreadPool()
{
    assert(threads_.empty());
}

void tp::ThreadPool::Start()
{
    pool_active_ = true;
    for (size_t i = 0; i < threads_.capacity(); ++i)
    {
        threads_.push_back(std::thread([&](){
            g_LocalThreadPool = this;

            while(pool_active_)
            {
                std::optional<Task> task = queue_.Take();
                if (task.has_value())
                {
                    task.value()();
                    task_count_.fetch_sub(1);
                }
            }
        }));
    }
}

void tp::ThreadPool::Submit(Task task)
{
    queue_.Put(std::move(task));
    task_count_.fetch_add(1);
}

tp::ThreadPool* tp::ThreadPool::Current()
{
    return g_LocalThreadPool;
}

void tp::ThreadPool::WaitIdle()
{
    while(task_count_.load() > 0)
    {
        ;
    }
        
    std::cout << "Thread pool is idle" << std::endl;
}

void tp::ThreadPool::Stop()
{
    pool_active_ = false;
    queue_.Close();

    std::cout << "Stopped. Joining threads" << std::endl;
    for (std::thread& thread : threads_)
    {
        thread.join();
    }

    std::cout << "Clean-up" << std::endl;
    threads_.clear();
}
