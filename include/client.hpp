#ifndef BRETLNET_CLIENT_H
#define BRETLNET_CLIENT_H

#include "common.hpp"

class Client {
    private:
        int sockfd;
        Protocol proto;
        char *connectAddr;
        int port;
        struct sockaddr ai_addr;
    public:
        Client(Protocol p, const char *connectAddr, int portNum);
        ~Client();
        void Connect();
        int Send(const char *data, int len);
};

#endif
