#ifndef BRETLNET_CLIENT_H
#define BRETLNET_CLIENT_H

#include "common.hpp"

class Client : private BretlNetService {
    public:
        enum Protocol {TCP, CONN_UDP, UDP};
        Client(Protocol p, const char *connectAddr, int portNum);
        ~Client();
        void Connect();
        int Send(const char *data, int len);
    private:
        char *connectAddr;
        struct sockaddr ai_addr; // for UDP clients
        Protocol proto;
};

#endif
