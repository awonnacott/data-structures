CC = g++
CFLAGS = -std=c++2a -fconcepts -Wall -Wextra -pedantic -g --coverage
COV = gcov
COVFLAGS = -jmafr

all: tests recursive_iterate

test: all
	./tests

recursive_iterate: recursive_iterate.o
	$(CC) $(CFLAGS) $< -o $@

recursive_iterate.o: recursive_iterate.cpp recursive_iterate.hpp
	$(CC) $(CFLAGS) $< -c

tests: tests_main.o tests_recursive_iterate.o
	$(CC) $(CFLAGS) $^ -o $@

tests_main.o: tests_main.cpp
	$(CC) $(CFLAGS) $< -c

tests_%.o: tests_%.cpp recursive_iterate.hpp
	$(CC) $(CFLAGS) $< -c

clean:
	rm -rf tests recursive_iterate *.o *.gcda *.gcno *.gcov

cov: recursive_iterate.hpp.gcov

recursive_iterate.hpp.gcov: test recursive_iterate
	./recursive_iterate
	$(COV) $(COVFLAGS) tests_recursive_iterate.cpp
