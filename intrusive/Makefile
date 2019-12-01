CC = clang++
CFLAGS = -std=c++2a -Wall -Wextra -pedantic -g --coverage
COV = llvm-cov gcov
COVFLAGS = -af

all: tests

test: all
	./tests

tests: tests_main.o tests_pair.o
	$(CC) $(CFLAGS) $^ -o $@

tests_main.o: tests_main.cpp
	$(CC) $(CFLAGS) $< -c

tests_%.o: tests_%.cpp pair.hpp
	$(CC) $(CFLAGS) $< -c

clean:
	rm -f tests *.o *.gcov *.gcda *.gcno

cov: pair.hpp.gcov

pair.hpp.gcov: test
	$(COV) $(COVFLAGS) tests_pair.cpp
