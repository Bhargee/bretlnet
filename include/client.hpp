#ifndef BRETLNET_CLIENT_H
#define BRETLNET_CLIENT_H

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <exception>

enum Protocol {TCP, UDP, CONN_UDP};

class ClientException : public std::exception {
    public:
        ClientException(const char *msg, bool sysMsg=false) throw();
        ~ClientException() throw();
        const char *what() const throw();
    private:
        std::string userMessage;
};

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
