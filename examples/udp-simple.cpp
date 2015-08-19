#include <stdio.h>
#include <functional>
#include <unistd.h>
#include <cassert>
#include "server.hpp"

void callback (char *) {
    printf("Hello, world!\n");    
}

int main() {
    Server s(Server::Protocol::UDP, 3456, 14, callback);
    s.Serve();
    return 0;
}
