#include <stdio.h>
#include "client.hpp"

int main() {
    Client c(Client::Protocol::UDP, "127.0.0.1", 3456);
    c.Connect();
    c.Send("Hello, world\0", 13);
    return 0;
}
