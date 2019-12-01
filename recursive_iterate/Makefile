CC = g++
CFLAGS = -std=c++2a -fconcepts -Wall -Wextra -pedantic -g --coverage

all: tests recursive_iterate

test: recursive_iterate.hpp.gcov

recursive_iterate: recursive_iterate.cpp
	$(CC) $(CFLAGS) recursive_iterate.cpp -o recursive_iterate

tests: tests_main.o tests_recursive_iterate.o
	$(CC) $(CFLAGS) tests_main.o tests_recursive_iterate.o -o tests

tests_main.o: tests_main.cpp
	$(CC) $(CFLAGS) tests_main.cpp -c

tests_recursive_iterate.o: tests_recursive_iterate.cpp recursive_iterate.hpp
	$(CC) $(CFLAGS) tests_recursive_iterate.cpp -c

clean:
	rm -rf tests recursive_iterate *.o *.gcda *.gcno *.gcov

recursive_iterate.hpp.gcov: all
	./tests
	./recursive_iterate
	gcov tests_recursive_iterate.cpp -jmafr
