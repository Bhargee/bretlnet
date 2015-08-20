#ifndef BRETLNET_CLIENT_H
#define BRETLNET_CLIENT_H

#include <stdlib.h>
#include <exception>
extern "C" {
    #include <libmill.h>
}

#define BACKLOG 20

class Client {
    public:
        // API
        enum Protocol {TCP, CONN_UDP, UDP};
        Client(Protocol p, const char *connectAddr, int portNum);
        ~Client();
        void Connect();
        //void Send(const std::vector<char> &data);
        void Send(const unsigned char *data, int len);
    private:
        void SendCoro(const unsigned char *data, int len);
        Protocol proto;
        ipaddr remoteAddr;
        udpsock udps;
        tcpsock tcps;
};

inline Client::Client(Protocol p, const char *c, int portNum) :
    proto(p) {
        remoteAddr = ipremote(c, portNum, 0, -1);
}

inline Client::~Client() {}

inline void Client::Connect() {
    ipaddr localAddr = iplocal(NULL, 0, IPADDR_PREF_IPV4);
    if (this->proto == UDP) {
        udps = udplisten(localAddr);
        if (!udps) 
           throw std::runtime_error(strerror(errno));
    }
    else {
        tcps = tcplisten(localAddr, BACKLOG);
        if (!tcps)
            throw std::runtime_error(strerror(errno));
    }
}
    
inline void Client::SendCoro(const unsigned char *data, int len) {
    if (this->proto == UDP) {
        udpsend(udps, remoteAddr, data, len);
    }
    else {
        tcpsend(tcps, data, len, -1);
        tcpflush(tcps, -1);
    }
    if (errno)
        perror("error sending packet");
}


inline void Client::Send(const unsigned char *data, int len) {
    go(SendCoro(data, len));
}

#endif
