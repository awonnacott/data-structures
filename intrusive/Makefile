COV = llvm-cov gcov
CCOVFLAGS = -g -O0 -fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage
# --coverage
COVFLAGS = -af

CC = clang++
CFLAGS = -std=c++2a -Wall -Wextra -pedantic -g

all: tests

test: all
	./tests

tests: tests_main.o tests_pair.o
	$(CC) $(CFLAGS) $(CCOVFLAGS) $^ -o $@

tests_main.o: tests_main.cpp
	$(CC) $(CFLAGS) $< -c

tests_%.o: tests_%.cpp pair.hpp
	$(CC) $(CFLAGS) $(CCOVFLAGS) $< -c

clean:
	rm -f tests *.o *.gcov *.gcda *.gcno

cov: pair.hpp.gcov

pair.hpp.gcov: test
	$(COV) $(COVFLAGS) tests_pair.cpp
