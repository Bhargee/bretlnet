#include <cstring>
#include "threadpool.hpp"

ThreadPool::ThreadPool(size_t n, const std::function <void(char *)> onPacket) {
    this->nThreads = n;
    this->task = onPacket;
    this->stop = false;
    for (size_t i = 0; i < n; ++i) 
        workers.emplace_back(
            [this]
            {
                for (;;)
                {
                    char *data;
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
                    free(data);
                }
            }
        );
}

ThreadPool::ThreadPool(size_t n) {
    this->nThreads = n;
    this->stop = false;
}

void ThreadPool::Push(char *data, size_t len) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) {
            throw std::runtime_error("push on stopped thread pool");
        }
        char *copied = (char *)malloc(len);
        std::memcpy(copied, data, len);
        dataQueue.emplace(copied);
    }
    condition.notify_one();
}

void ThreadPool::Push(std::function <void()> poolTask) {
    if (stop) {
        throw std::runtime_error("push on stopped thread pool");
    }
    workers.emplace_back([poolTask](){poolTask();});
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

