#include "client.hpp"

Client::Client(Protocol p, const char *connectAddr, int portNum) : BretlNetService(portNum) {
    this->proto = p;
    size_t add_len = strlen(connectAddr) + 1;
    this->connectAddr = (char *)malloc(add_len);
    memcpy(this->connectAddr, connectAddr, add_len);
}

Client::~Client() {
    free(this->connectAddr);
}

void Client::Connect() {
    // for constructing an exception, if need be
    const char *error_msg;
    // standard UNIX socket stuff
    struct addrinfo hints, *servinfo, *p;
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = (this->proto == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM);

    // populate struct servinfo for use in later calls
    std::string port_str = std::to_string(this->port);
    if ((rv = getaddrinfo(this->connectAddr, port_str.c_str(), &hints, &servinfo)) != 0) {
        error_msg = gai_strerror(rv);
        goto early_error;
    }

    // traverse servinfo linked list, use first descriptor that works
    // TODO find out what this means
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            continue;
        }

        // If the client proto is connected, do connect syscall
        if (this->proto != Protocol::UDP) {
            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                sockfd = -1;
                continue;
            }
        }

        break;
    }

    if (p == NULL) {
        error_msg = "failed to connect\n";
        goto error;
    }

    memmove(&this->ai_addr, p->ai_addr, p->ai_addrlen);
    freeaddrinfo(servinfo);
    return;

error:
    close(sockfd);
    sockfd = -1; //fallthrough to early_error
early_error:
    freeaddrinfo(servinfo);
    throw std::runtime_error(error_msg);
}

int Client::Send(const char *data, int len) {
    int numbytes;
    if (this->proto == Protocol::UDP) {
        if ((numbytes = sendto(this->sockfd, data, len, 0,
             &this->ai_addr, sizeof this->ai_addr)) == -1) {
            goto error;
       }
    }
    else {
        if ((numbytes = send(this->sockfd, data, len, 0)) == -1)
            goto error;
    }

    return numbytes;

error:
    std::string err_msg ("send failed - ");
    err_msg += strerror(errno);
    throw std::runtime_error(err_msg.c_str());
}
