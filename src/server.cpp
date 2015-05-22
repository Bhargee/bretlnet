#include "server.hpp"

Server::Server(Protocol p, int portNum, int buflen) : BretlNetService(portNum) {
    this->bufsize = (p != Protocol::TCP) ? std::min(buflen, (2 << 16) - 8) : buflen;
    this->proto = p;
}

Server::~Server() {
}

void Server::Serve(void (*f)(char *)) {
    // TODO add support for *UDP
    // TODO make async
    int new_fd; 
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes=1;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string port_str = std::to_string(this->port);
    if ((rv = getaddrinfo(NULL, port_str.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        //return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        //return 2;
    }

    freeaddrinfo(servinfo);

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    char buf[this->bufsize];

    while(1) {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        if (recv(new_fd, buf, this->bufsize, 0) == -1) {
            perror("recv");
            exit(1);
        }
        f(buf);
    }
}
