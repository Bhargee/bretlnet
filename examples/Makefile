# this makefile builds the binary specified on the cmd line
# which should be the name of a cpp file without the .cpp
# if none is provided it executes the clean target
SHELL = /bin/sh
CC = g++-4.8
CPPFLAGS = -I../ -std=c++11 -L/usr/local/lib -lmill -pedantic -Wall -Wextra -O3
DEBUG = -g
SOURCES = $(shell echo *.cpp)
OBJECTS = $(SOURCES:.cpp=)
OPENCV = `pkg-config --libs --cflags opencv`
RASPI = -I/usr/local/include -lraspicam -lraspicam_cv -L/opt/vc/lib -lmmal -lmmal_core -lmmal_core -lmmal_util -lopencv_core -lopencv_highgui -lopencv_imgproc

nothing:
	@echo "please follow make cmd with 'clean' or filename"

raspi-camera-client: raspi-camera-client.cpp
	$(CC) $< -o $@  $(CPPFLAGS) $(RASPI)

clean:
	-rm -f $(OBJECTS)

$(filter-out clean raspi-camera-client, $(MAKECMDGOALS)): $(MAKECMDGOALS).cpp ../server.hpp ../client.hpp
	$(CC)  $< -o $@ $(OPENCV) $(CPPFLAGS)

 
.PHONY: clean nothing
