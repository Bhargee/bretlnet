# this makefile builds the binary specified on the cmd line
# which should be the name of a cpp file without the .cpp
# if none is provided it executes the clean target
SHELL = /bin/sh
CC = g++
CPPFLAGS = -I../ -std=c++11 -lmill -pedantic -Wall -Wextra -O3
DEBUG = -g
SOURCES = $(shell echo *.cpp)
OBJECTS = $(SOURCES:.cpp=)

$(MAKECMDGOALS): $(MAKECMDGOALS).cpp ../server.hpp ../client.hpp
	$(CC)  $< -o $@ $(CPPFLAGS)
clean:
	-rm -f $(OBJECTS)
 
.PHONY: clean
