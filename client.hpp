#ifndef BRETLNET_CLIENT_H
#define BRETLNET_CLIENT_H

#include <thread>
#include <vector>
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


class Client {
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
        int port, sockfd;
        // implementation
        std::vector<std::thread> activeThreads; // to simulate asynchronicity
        void signalSendErr(int &numbytes);
};

#endif
