#include <stdio.h>
#include <functional>
#include <unistd.h>
#include <cassert>
#include "server.hpp"
#include "client.hpp"

/* simple UDP send/recv example - 
 * In this, and all other, examples, 
 */

using namespace std;

static bool serveTraffic = true;

/* This code creates a simple UDP server */
function<void()> server = [](){
    Server::Protocol proto = Server::Protocol::UDP;

    int port = 3456;

    int numThreads = 3; //# of threads in threadpool, should be, at minimum, # of cores

    int dataLen = 14; // length of expected data, in bytes

    function <void(vector<char>)> callback = // callback must have this signature
        [](std::vector<char> v) { // the received data is accessed as a vector
            string str(v.begin(),v.end()); 
            cout << str << endl;
            serveTraffic = false;
    };

    Server s(proto, port, numThreads, dataLen, callback);
    s.Serve(false); // 'true' creates nonblocking server, false creates blocking 
};

function<void()> client = [](){
    Client c (Client::UDP,"127.0.0.1", 3456); // protocol, dest addr, dest port
    c.Connect(); // must call connect before attempting to send!
    c.Send("Hello, World!", 14); // can also call Send with 1 arg, a vector with data
};

int main() {
    thread t(server);
    client();
    while(serveTraffic);
    return 0;
}
