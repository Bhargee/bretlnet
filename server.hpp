#ifndef BRETLNET_SERVER_H
#define BRETLNET_SERVER_H

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
#include <cstring>
#include <stdlib.h>

#define BACKLOG 20

class Server {
    public:
        enum Protocol {TCP, UDP};
        Server(Protocol p, int portNum, int numThreads, size_t dataLen, 
                std::function<void(std::vector<char>)> onPacket);
        ~Server();
        void Serve(bool async);

    private:
        class ThreadPool {
            public:
                ThreadPool(size_t nThreads, 
                        const std::function <void(std::vector<char>)> onPacket);
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

        Protocol proto;
        ThreadPool *workerPool;
};

inline Server::ThreadPool::ThreadPool(size_t n, 
        const std::function <void(std::vector<char>)> onPacket) {
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
                            [this]{ return this->stop || 
                                    !this->dataQueue.empty();});
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

inline Server::ThreadPool::ThreadPool(size_t n) {
    this->nThreads = n;
    this->stop = false;
}

inline void Server::ThreadPool::Push(std::vector<char> data) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) {
            throw std::runtime_error("push on stopped thread pool");
        }
        dataQueue.emplace(std::move(data));
    }
    condition.notify_one();
}

inline void Server::ThreadPool::Push(std::function <void()> poolTask) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) {
            throw std::runtime_error("push on stopped thread pool");
        }
        workers.emplace_back([poolTask](){poolTask();});
    }
    condition.notify_one();
}

inline Server::ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}

#endif
