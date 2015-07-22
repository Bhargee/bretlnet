#include <cstring>
#include "threadpool.hpp"

ThreadPool::ThreadPool(size_t n, const std::function <void(std::vector<char>)> onPacket) {
    this->nThreads = n;
    this->task = std::move(onPacket);
    this->stop = false;
    for (size_t i = 0; i < n; ++i) 
        workers.emplace_back(
            [this]
            {
                for (;;)
                {
                std::vector<char> data;
                    {
                        std::unique_lock<std::mutex> lock (this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->dataQueue.empty();});
                        if (this->stop && this->dataQueue.empty())
                            return;
                        data = this->dataQueue.front();
                        this->dataQueue.pop();
                    }
                    this->task(data);
                }
            }
        );
}

ThreadPool::ThreadPool(size_t n) {
    this->nThreads = n;
    this->stop = false;
}

void ThreadPool::Push(std::vector<char> data) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) {
            throw std::runtime_error("push on stopped thread pool");
        }
        dataQueue.emplace(std::move(data));
    }
    condition.notify_one();
}

void ThreadPool::Push(std::function <void()> poolTask) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) {
            throw std::runtime_error("push on stopped thread pool");
        }
        workers.emplace_back([poolTask](){poolTask();});
    }
    condition.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}
