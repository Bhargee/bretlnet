#ifndef _BRETLNET_THREADPOOL_H
#define _BRETLNET_THREADPOOL_H

#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <exception>
#include <future>
#include <mutex>
#include <queue>
#include <iostream>

class ThreadPool {
    public:
        ThreadPool(size_t nThreads, const std::function <void(std::vector<char>)> onPacket);
        ThreadPool(size_t nThreads);
        ~ThreadPool();
        void Push(std::vector<char> buf);
        void Push(std::function <void()> poolTask);
    private:
        size_t nThreads;

        std::function <void(std::vector<char>)> task;
        std::queue<std::vector<char>> dataQueue;

        std::vector< std::thread > workers;
         // synchronization
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
};

#endif
