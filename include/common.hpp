#ifndef BRETLNET_COMMON_H
#define BRETLNET_COMMON_H

//#include <iostream>
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
// for nice c++11 exceptions
#include <functional>

class BretlNetService {
    protected:
        BretlNetService(int portNum);
        ~BretlNetService();
        int sockfd;
        int port;
};

inline BretlNetService::BretlNetService(int portNum) {
    this->port = portNum;
    this->sockfd = -1;
}

inline BretlNetService::~BretlNetService() {
    close(this->sockfd);
}

#endif
