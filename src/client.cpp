#include "client.hpp"

ClientException::ClientException(const char *message, bool sysMsg)
    throw() : userMessage(message) {
        if (sysMsg) {
            userMessage.append(": ");
            userMessage.append(strerror(errno));
        }
    }

const char *ClientException::what() const throw() {
    return userMessage.c_str();
}

Client::Client(Protocol p, const char *connectAddr, int portNum) {
    this->proto = p;

    this->connectAddr = (char *)malloc(sizeof(connectAddr));
    memcpy(this->connectAddr, connectAddr, sizeof(connectAddr));

    this->port = portNum;

    this->sockfd = -1;
}

Client::~Client() {
    close(this->sockfd);
    delete this->connectAddr;
}

void Client::Connect() {
    // for constructing a ClientException, if need be
    const char *error_msg;
    // standard UNIX socket stuff, like in beej
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

    goto done;

error:
    close(sockfd);
    sockfd = -1; //fallthrough -->
early_error:
    freeaddrinfo(servinfo);
    throw ClientException(error_msg);
done:
    memmove(&this->ai_addr, p->ai_addr, p->ai_addrlen);
    freeaddrinfo(servinfo);
    return;
}

int Client::Send(const char *data, int len) {
    int numbytes;
    if (this->proto == Protocol::UDP) {
        if ((numbytes = sendto(this->sockfd, data, len, 0,
             &this->ai_addr, sizeof this->ai_addr)) == -1) {
            throw ClientException("UDP send failed - ", true);
        }
    }
    else {
        if ((numbytes = send(this->sockfd, data, len, 0)) == -1)
           throw ClientException("Connected send failed - ", true); 
    }

    return numbytes;
}
