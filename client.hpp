#ifndef BRETLNET_CLIENT_H
#define BRETLNET_CLIENT_H

#include <stdexcept>
#include <string.h>
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
        void Send(const unsigned char *data, int len);
    private:
        void SendCoro(const unsigned char *data, int len);
        void raiseConnectErr();
        Protocol proto;
        ipaddr remoteAddr;
        udpsock udps = NULL;
        tcpsock tcps = NULL;
};

inline Client::Client(Protocol p, const char *c, int portNum) :
    proto(p) {
        remoteAddr = ipremote(c, portNum, 0, -1);
}

inline Client::~Client() {
    if (tcps)
        tcpclose(tcps);
    else if (udps)
        udpclose(udps);
}

inline void Client::raiseConnectErr() {
    char message[100]; //arbitrary
    strerror_r(errno, message, 100);
    throw std::runtime_error(message);
}

inline void Client::Connect() {
    if (this->proto == UDP) {
        ipaddr localAddr = iplocal(NULL, 0, IPADDR_PREF_IPV4);
        udps = udplisten(localAddr);
        if (!udps) 
           raiseConnectErr();
    }
    else {
        tcps = tcpconnect(remoteAddr, -1);
        if (!tcps)
            raiseConnectErr();
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
