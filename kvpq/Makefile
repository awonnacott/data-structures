CC = clang++
CFLAGS = -std=c++2a -Wall -Wextra -pedantic -g --coverage
COV = llvm-cov gcov
COVFLAGS = -af

all: tests

test: all
	./tests

tests: tests_main.o tests_kvpq.o tests_load_factor.o
	$(CC) $(CFLAGS) $^ -o $@

tests_main.o: tests_main.cpp
	$(CC) $(CFLAGS) $< -c

tests_load_factor.o: tests_load_factor.cpp kvpq.hpp
	$(CC) $(CFLAGS) -Wno-keyword-macro $< -c

tests_%.o: tests_%.cpp kvpq.hpp
	$(CC) $(CFLAGS) $< -c

clean:
	rm -f tests *.o *.gcov *.gcda *.gcno

cov: kvpq.hpp.gcov

kvpq.hpp.gcov: test
	$(COV) $(COVFLAGS) tests_kvpq.cpp
