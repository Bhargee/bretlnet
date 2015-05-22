#include "server.hpp"

Server::Server(Protocol p, int portNum, int numThreads) 
    : BretlNetService(portNum) {
    this->proto = p;
    this->nThreads = numThreads;
}

Server::~Server() {
}

// might throw exception
void Server::Init() {
    (this->proto == UDP) ? InitUDP() : InitTCP();
}

void Server::InitTCP() {
}

void Server::InitUDP() {
    struct addrinfo hints, *servinfo, *p;
    int rv;
    const char *error_msg;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(this->port);
    if ((rv = getaddrinfo(NULL, port_str.c_str(), &hints, &servinfo)) != 0) {
        error_msg = gai_strerror(rv);
        goto error;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        error_msg = "failed to bind socket\n";
        goto error;
    }

    freeaddrinfo(servinfo);
    return;

error:
    throw std::runtime_error(error_msg);
}

void Server::Serve(void (*f)(char *)) {
}
