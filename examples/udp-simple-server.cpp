#include <stdio.h>
#include <iostream>
#include "server.hpp"

using namespace std;

void callback (char *data) {
    cout << data << endl;    
}

int main() {
    Server s(Server::Protocol::UDP, 3456, 13, callback);
    s.Serve();
    return 0;
}
