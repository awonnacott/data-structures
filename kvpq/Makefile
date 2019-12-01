CC=g++
CFLAGS=-std=c++2a -Wall -Wextra -pedantic -g --coverage

all: tests

test: all
	./tests

tests: tests_main.o tests_kvpq.o
	$(CC) $(CFLAGS) tests_main.o tests_kvpq.o -o tests

tests_main.o: tests_main.cpp
	$(CC) $(CFLAGS) tests_main.cpp -c

tests_kvpq.o: tests_kvpq.cpp kvpq.hpp
	$(CC) $(CFLAGS) tests_kvpq.cpp -c

clean:
	rm -f tests *.o *.gcov *.gcda *.gcno

cov: kvpq.hpp.gcov

kvpq.hpp.gcov: test
	gcov tests_kvpq.cpp -jmafr
