#ifndef BRETLNET_CLIENT_H
#define BRETLNET_CLIENT_H

#include "common.hpp"
#include <thread>
#include <vector>

class Client : private BretlNetService {
    public:
        // API
        enum Protocol {TCP, CONN_UDP, UDP};
        Client(Protocol p, const char *connectAddr, int portNum);
        ~Client();
        void Connect();
        void Send(const std::vector<char> &data);
        void Send(const char *data, int len);
    private:
        std::string connectAddr;
        struct sockaddr ai_addr; // for UDP clients
        Protocol proto;
        // implementation
        std::vector<std::thread> activeThreads; // to simulate asynchronicity
};

#endif
