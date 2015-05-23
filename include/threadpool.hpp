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

class ThreadPool {
    public:
        ThreadPool(size_t nThreads, std::function <void(char*)> onPacket);
        ~ThreadPool();
        void Push(char *data, size_t len);
    private:
        size_t nThreads;
        std::function <void(char*)> task;
        std::vector< std::thread > workers;
        // where 'tasks' are recieved data buffers that need to be processed 
        // by this->task
        std::queue<char *> tasks; 
         // synchronization
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
};

inline ThreadPool::ThreadPool(size_t n, const std::function <void(char *)> onPacket) {
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
                            [this]{ return this->stop || !this->tasks.empty();});
                        if (this->stop && this->tasks.empty())
                            return;
                        data = this->tasks.front();
                        this->tasks.pop();
                    }
                    this->task(data);
                    free(data);
                }
            }
        );
}

inline void ThreadPool::Push(char *data, size_t len) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) {
            throw std::runtime_error("push on stopped thread pool");
        }
        char *copied = (char *)malloc(len);
        memcpy(copied, data, len);
        tasks.emplace(copied);
    }
    condition.notify_one();
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}
#endif
