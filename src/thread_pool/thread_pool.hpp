#pragma once

#include "queue.hpp"
#include "task.hpp"

namespace tp 
{
    class ThreadPool
    {
    public:
        explicit ThreadPool(size_t threads);
        ~ThreadPool();

        // Non-copyable
        ThreadPool(const tp::ThreadPool& other) = delete;
        ThreadPool& operator=(const tp::ThreadPool& other) = delete;

        // Non-movable
        ThreadPool(tp::ThreadPool&& other) = delete;
        ThreadPool& operator=(const ThreadPool&& other) = delete;

        // Launches worker threads
        void Start();

        // Schedules task for execution in one of the worker threads
        void Submit(Task task);

        // Locates current thread pool from worker thread
        static ThreadPool* Current();

        // Waits until outstanding work count reaches zero
        void WaitIdle();

        // Stops the worker threads as soon as possible
        void Stop();


        private:
        std::vector<std::thread> threads_;
        UnboundedBlockingQueue<Task> queue_{};

        std::atomic<int> task_count_ = 0;
        // Should be changed only in the main thread, thus unsafe bool
        bool pool_active_ = false;
    };
}