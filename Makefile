SHELL = /bin/sh
CC = clang++
CPPFLAGS = -std=c++11 -Iinclude  -pedantic -Wall -Wextra -O3
TARGET = libbretlnet.a
SOURCES = $(shell echo src/*.cpp)
HEADERS = $(shell echo include/*.h)
OBJECTS = $(SOURCES:.cpp=.o)

lib: $(TARGET)

$(TARGET): $(OBJECTS)
	-ar rcs $(TARGET) $(OBJECTS)

%.o: %.c $(HEADERS) $(COMMON)
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean:
	-rm -f $(OBJECTS)
 
distclean: clean
	-rm -f $(TARGET)

.PHONY: clean distclean lib
