#ifndef BRETLNET_SERVER_H
#define BRETLNET_SERVER_H

#include "common.hpp"
#include "threadpool.hpp"

class Server : private BretlNetService {
    public:
        enum Protocol {TCP, UDP};
        Server(Protocol p, int portNum, int numThreads, size_t dataLen, 
                std::function<void(std::vector<char>)> onPacket);
        ~Server();

        void Serve();
    private:
        Protocol proto;
        ThreadPool *workerPool;
        /* used for both TCP and UDP, may change this to a vector of threads
           because if # of worker threads >> # of listener threads
           lots of worker thread time will be wasted blocking, more dropped 
           packets/reduced throughput */
        std::thread *listenThread;
        // in bytes
        size_t dataLen;

        std::function<void(std::vector<char>)> onTCP;

        // do socket init (socket, bind, listen calls)
        void Init();
        // delegates for serving UDP or TCP traffic
        void ServeUDP();
        void ServeTCP();
};

#endif
