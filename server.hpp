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
// socket stuff
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


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
        std::thread *listenThread;
        // in bytes
        size_t dataLen;
        int sockfd;
        int port;

        std::function<void(std::vector<char>)> onTCP;

        // do socket init (socket, bind, listen calls)
        void Init();
        // delegates for serving UDP or TCP traffic
        void ServeUDP();
        void ServeTCP();
};

inline Server::Server(Protocol p, int portNum, int numThreads, size_t dataLen,
        const std::function<void(std::vector<char>)> onPacket) {
    this->proto = p;
    this->sockfd = -1;
    this->port = portNum;
    if (p == UDP)
        this->workerPool = new ThreadPool(numThreads, std::move(onPacket));
    else {
        this->workerPool = new ThreadPool(numThreads);
        this->onTCP = std::move(onPacket);
    }
    this->dataLen = dataLen;
}

inline Server::~Server() {
    delete this->workerPool; 
    this->listenThread->join();
    delete this->listenThread;
    this->sockfd = -1;
}

inline void Server::Init() {
    struct addrinfo hints, *servinfo, *p;
    int rv;
    const char *error_msg;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = (this->proto == UDP) ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(this->port);
    if ((rv = getaddrinfo(NULL, port_str.c_str(), &hints, &servinfo)) != 0) {
        error_msg = gai_strerror(rv);
        goto error;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            continue;
        }

        if (this->proto == TCP) {
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
                    goto error;
            }
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        error_msg = "failed to bind socket\n";
        goto error;
    }

    freeaddrinfo(servinfo);
    if (this->proto == TCP) {
        if (listen(sockfd, BACKLOG) == -1) {
            close(sockfd);
            goto error;
        }
    }
    return;

error:
    std::string err (strerror(errno));
    err += error_msg;
    sockfd = -1;
    throw std::runtime_error(err.c_str());

}

inline void Server::Serve(bool async) {
    if (this->sockfd == -1) {
        Init();
    }

    if (async) {
        this->listenThread = new std::thread([this]() {
            (this->proto == UDP) ? this->ServeUDP() : this->ServeTCP();
        });
    }
    else {
        (this->proto == UDP) ? this->ServeUDP() : this->ServeTCP();
    }
}

inline void Server::ServeUDP() {
    std::vector<char> buf(this->dataLen);
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;

    while (recvfrom(this->sockfd, buf.data(), this->dataLen, 0,
                (struct sockaddr *) &their_addr, &addr_len) != -1) {
        this->workerPool->Push(buf);
    }
}

inline void Server::ServeTCP() {
   this->listenThread = new std::thread(
       [this] {
           struct sockaddr_storage their_addr;
           socklen_t addr_len = sizeof their_addr;
           int clientfd;
           std::function <void()> task;
           
           for (;;) {
               // block here
               clientfd = accept(this->sockfd, (struct sockaddr *) &their_addr,
                       &addr_len);
               if (clientfd == -1)
                   continue;
               
               task = [this, clientfd] {
               std::vector <char> buf(this->dataLen);
                   int ret;
                   for (;;) {
                       ret = recv(clientfd, buf.data(), this->dataLen, 0);
                       if (ret <= 0)
                           break;
                       this->onTCP(buf);
                   }
               };
               this->workerPool->Push(std::move(task));
           }
       });
}

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
