#ifndef BRETLNET_SERVER_H
#define BRETLNET_SERVER_H

#include "common.hpp"
//#include "threadpool.hpp"

class Server : private BretlNetService {
    public:
        enum Protocol {TCP, UDP};
        Server(Protocol p, int portNum, int numThreads);
        ~Server();
        void Serve(void (*f)(char *));
    private:
        Protocol proto;
        int nThreads;
//        ThreadPool workerPool;
        // do socket init (socket, bind, listen)
        void InitUDP();
        void InitTCP();
        void Init();
        // pthread listener?
};



#endif
