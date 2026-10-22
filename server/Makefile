CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

all: server

server: server.cpp
	$(CXX) $(CXXFLAGS) server.cpp -o server

clean:
	rm -f server

.PHONY: all clean
