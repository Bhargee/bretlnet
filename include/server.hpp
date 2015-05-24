#ifndef BRETLNET_SERVER_H
#define BRETLNET_SERVER_H

#include "common.hpp"
#include "threadpool.hpp"

class Server : private BretlNetService {
    public:
        enum Protocol {TCP, UDP};
        Server(Protocol p, int portNum, int numThreads, size_t dataLen, 
                std::function<void(char *)> onPacket);
        ~Server();

        void Serve();
    private:
        Protocol proto;
        ThreadPool *workerPool;
        std::thread *listenThread;
        // in bytes
        size_t dataLen;

        // do socket init (socket, bind, listen calls)
        void Init();
};

#endif
