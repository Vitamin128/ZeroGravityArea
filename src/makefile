cc=g++
FLAGS=-std=c++17 -I. -I../cppjieba/include -I../cppjieba/deps/limonp/include

all: main
main: ./main.cc
	$(cc) -o $@ $^ $(FLAGS) -lboost_system -lboost_filesystem -ljsoncpp

.PHONY: clean
clean:
	rm -f main
