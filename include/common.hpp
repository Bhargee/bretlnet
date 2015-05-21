#ifndef BRETLNET_COMMON_H
#define BRETLNET_COMMON_H

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

#include <functional>

enum Protocol {TCP, UDP, CONN_UDP};

class NetException : public std::exception {
    public:
        NetException(const char *msg, bool sysMsg=false) throw();
        ~NetException() throw();
        const char *what() const throw();
    private:
        std::string userMessage;
};

NetException::NetException(const char *message, bool sysMsg)
    throw() : userMessage(message) {
        if (sysMsg) {
            userMessage.append(": ");
            userMessage.append(strerror(errno));
        }
    }

const char *NetException::what() const throw() {
    return userMessage.c_str();
}

typedef NetException ClientException;
typedef NetException ServerException ;

#endif
