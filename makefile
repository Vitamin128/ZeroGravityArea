cc=g++
FLAGS=-std=c++11 -I. -I./cppjieba/include -I./cppjieba/deps/limonp/include

all: parser

parser: ./src/parser.cc
	$(cc) -o $@ $^ $(FLAGS) -lboost_system -lboost_filesystem

.PHONY: clean
clean:
	rm -f parser 
