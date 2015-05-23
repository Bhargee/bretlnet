#ifndef BRETLNET_SERVER_H
#define BRETLNET_SERVER_H

#include "common.hpp"
#include "threadpool.hpp"

class Server : private BretlNetService {
    public:
        enum Protocol {TCP, UDP};
        Server(Protocol p, int portNum, int numThreads, 
                std::function<void(char *)> onPacket, size_t dataLen);
        ~Server();
        void Serve();
    private:
        Protocol proto;
        ThreadPool *workerPool;
        std::function<void(char *)> task;
        std::thread listenThread;
        // in bytes
        size_t dataLen;
        // do socket init (socket, bind, listen)
        void InitUDP();
        void InitTCP();
        void Init();
};

#endif
