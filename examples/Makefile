# this makefile builds the binary specified on the cmd line
# which should be the name of a cpp file without the .cpp
# if none is provided it executes the clean target
SHELL = /bin/sh
CC = g++
CPPFLAGS = -I../ -std=c++11 -lmill -pedantic -Wall -Wextra -O3
DEBUG = -g
SOURCES = $(shell echo *.cpp)
OBJECTS = $(SOURCES:.cpp=)
OPENCV = `pkg-config --libs --cflags opencv`

nothing:
	@echo "please follow make cmd with 'clean' or filename"

clean:
	-rm -f $(OBJECTS)

$(filter-out clean, $(MAKECMDGOALS)): $(MAKECMDGOALS).cpp ../server.hpp ../client.hpp
	$(CC)  $< -o $@ $(OPENCV) $(CPPFLAGS)

 
.PHONY: clean nothing
