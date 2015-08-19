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

extern "C" {
    #include <libmill.h>
}

#define BACKLOG 20

class Server {
    public:
        enum Protocol {TCP, UDP};
        Server(Protocol p, unsigned short portNum, size_t dataLen, 
                void (*callback) (char *));
        ~Server();
        void Serve(bool blocking);

    private:
        unsigned short port;
        Protocol proto;
        void (*callback) (char *);
        size_t packetSize;

        std::thread *asyncServe = NULL;
        std::function<void()> handleUDP = 
            [this]() {
                ipaddr addr = iplocal(NULL, this->port, 0);
                udpsock s = udplisten(addr);
                char buf[256];
                ipaddr a;
                for(;;) {
                    size_t sz = udprecv(s, &a, buf, this->packetSize, -1);
                    this->callback(buf);
                }
                udpclose(s);
            };
};

/* Server Impl */
inline Server::Server(Protocol p, unsigned short portNum, size_t dataLen, void (*cb) (char *)) :
    port(portNum),
    proto(p),
    callback(cb),
    packetSize(dataLen)
{
}

inline Server::~Server() {
    if (this->asyncServe) {
        asyncServe->join();
        delete asyncServe;
    }
}

inline void Server::Serve(bool blocking) {
    if (blocking) {
        handleUDP();
    } 
    else {
        asyncServe = new std::thread(handleUDP);
    }

}

#endif
