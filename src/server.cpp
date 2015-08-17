#include "server.hpp"
#define BACKLOG 20

Server::Server(Protocol p, int portNum, int numThreads, size_t dataLen,
        const std::function<void(std::vector<char>)> onPacket)
    : BretlNetService(portNum) {
    this->proto = p;
    if (p == UDP)
        this->workerPool = new ThreadPool(numThreads, std::move(onPacket));
    else {
        this->workerPool = new ThreadPool(numThreads);
        this->onTCP = std::move(onPacket);
    }
    this->dataLen = dataLen;
}

Server::~Server() {
    delete this->workerPool; 
    this->listenThread->join();
    delete this->listenThread;
}

void Server::Init() {
    struct addrinfo hints, *servinfo, *p;
    int rv;
    const char *error_msg;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = (this->proto == UDP) ? SOCK_DGRAM : SOCK_STREAM;
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

        if (this->proto == TCP) {
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
                    goto error;
            }
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
    if (this->proto == TCP) {
        if (listen(sockfd, BACKLOG) == -1) {
            close(sockfd);
            goto error;
        }
    }
    return;

error:
    std::string err (strerror(errno));
    err += error_msg;
    sockfd = -1;
    throw std::runtime_error(err.c_str());

}

void Server::Serve(bool async) {
    if (this->sockfd == -1) {
        Init();
    }

    if (async) {
        this->listenThread = new std::thread([this] {
            (this->proto == UDP) ? this->ServeUDP() : this->ServeTCP();
        });
    }
    else {
        (this->proto == UDP) ? this->ServeUDP() : this->ServeTCP();
    }
}

void Server::ServeUDP() {
    std::vector<char> buf(this->dataLen);
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;

    while (recvfrom(this->sockfd, buf.data(), this->dataLen, 0,
                (struct sockaddr *) &their_addr, &addr_len) != -1) {
        this->workerPool->Push(buf);
    }
}

void Server::ServeTCP() {
   this->listenThread = new std::thread(
       [this] {
           struct sockaddr_storage their_addr;
           socklen_t addr_len = sizeof their_addr;
           int clientfd;
           std::function <void()> task;
           
           for (;;) {
               // block here
               clientfd = accept(this->sockfd, (struct sockaddr *) &their_addr,
                       &addr_len);
               if (clientfd == -1)
                   continue;
               
               task = [this, clientfd] {
               std::vector <char> buf(this->dataLen);
                   int ret;
                   for (;;) {
                       ret = recv(clientfd, buf.data(), this->dataLen, 0);
                       if (ret <= 0)
                           break;
                       this->onTCP(buf);
                   }
               };
               this->workerPool->Push(std::move(task));
           }
       });
}
