#ifndef BRETLNET_SERVER_H
#define BRETLNET_SERVER_H

#include <stdlib.h>

extern "C" {
    #include <libmill.h>
}

#define BACKLOG 20

class Server {
    public:
        enum Protocol {TCP, UDP};
        Server(Protocol p, unsigned short portNum, size_t dataLen, 
                void (*callback) (char *));
        ~Server();
        void Serve();

    private:
        unsigned short port;
        Protocol proto;
        void (*callback) (char *);
        size_t packetSize;
        bool stop = false;
        
        void serveUDP(ipaddr &localAddr);
        void serveTCP(ipaddr &localAddr);
};

/* Server Impl */
inline Server::Server(Protocol p, unsigned short portNum, size_t dataLen, void (*cb) (char *)) :
    port(portNum), proto(p), callback(cb), packetSize(dataLen) {
}

inline Server::~Server() {
    stop = true;
}

inline void Server::Serve() {
    ipaddr localAddr = iplocal(NULL, this->port, IPADDR_PREF_IPV4);

    (this->proto == UDP) ? serveUDP(localAddr) : serveTCP(localAddr);

}

void Server::serveUDP (ipaddr &localAddr) {
    udpsock serverSock = udplisten(localAddr);
    if (!serverSock) {
        perror("Could not open UDP socket");
        goto cleanup; // TODO think about throwing an exception here?
    }

    char buf[this->packetSize];
    ipaddr remote;

    for(;;) {
        udprecv(serverSock, &remote, buf, this->packetSize, -1);
        if (errno) {
            perror("Received corrupt packet");
            goto cleanup;
        }
        go(this->callback(buf));
    }
cleanup:
    udpclose(serverSock);
}

void Server::serveTCP (ipaddr &localAddr) {
    tcpsock serverSock = tcplisten(localAddr, BACKLOG);
    if (!serverSock) {
        perror("Could not open TCP socket");
        goto cleanup;
    }

    for(;;) {
        tcpsock as = tcpaccept(serverSock, -1);
        if (!as)
            continue;
        //go(this->); // TODO
    }
cleanup:
   tcpclose(serverSock); 
}

#endif
