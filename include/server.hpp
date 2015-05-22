#ifndef BRETLNET_SERVER_H
#define BRETLNET_SERVER_H

#include "common.hpp"

class Server : private BretlNetService {
    public:
        enum Protocol {TCP, UDP};
        Server(Protocol p, int portNum, int buflen);
        ~Server();
        void Serve(void (*f)(char *));
    private:
        int bufsize;
        Protocol proto;
};

#endif
