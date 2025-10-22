cc=g++
all:parser server

parser: parser.cc
	$(cc) -o $@ $< -std=c++11 -I. -lboost_system -lboost_filesystem
server:server.cc
	$(cc) -o $@ $< -std=c++11 -I. -ljsoncpp
PHONY: clean
clean:
	rm -f parser
	rm -f server
