CC=g++
CFLAGS=-std=c++2a -Wall -Wextra -pedantic -g

all: tests

tests: tests_main.o tests_pair.o
	$(CC) $(CFLAGS) tests_main.o tests_pair.o -o tests

tests_main.o: tests_main.cpp
	$(CC) $(CFLAGS) tests_main.cpp -c

tests_pair.o: tests_pair.cpp pair.hpp
	$(CC) $(CFLAGS) tests_pair.cpp -c

clean:
	rm -f tests *.o

test: all
	./tests
