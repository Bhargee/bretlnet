#include "server.hpp"

Server::Server(Protocol p, int portNum, int buflen) : BretlNetService(portNum) {
    this->bufsize = (p != Protocol::TCP) ? std::min(buflen, (2 << 16) - 8) : buflen;
    this->proto = p;
}

Server::~Server() {
}

void Server::Serve(void (*f)(char *)) {
    // TODO add support for TCP
    // TODO make async
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    std::string port_str = std::to_string(this->port);
    if ((rv = getaddrinfo(NULL, port_str.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        //return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        //return 2;
    }

    freeaddrinfo(servinfo);

    char buf[this->bufsize];

    addr_len = sizeof their_addr;
    while(1) {
        if ((numbytes = recvfrom(this->sockfd, buf, this->bufsize , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        } 
        f(buf);
    }
}
