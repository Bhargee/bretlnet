## Bretlnet

A networking library for use on controllers in the Bretl lab. There are two
versions:  

1. From scratch, header only  
2. Based on boost.asio  

The former is completely from scratch and minimal. Simply include server.hpp or
client.hpp, depending on your needs. Usage is extremely simple - to figure it
out, read the comments and look under `examples`, which contains examples that
double as unit tests

The latter is a work in progress, started because I wanted to - 

* learn boost
* improve on the first effort vis-a-vis code quality and 'real' asynchronicity
  (I use threads in (1) to achieve the same effects as using epoll et all)

**NOTE** You can modify the makefile in `examples` to use g++ instead of
clang++ by changing `CC=clang++` to `CC+g++`
