COV = llvm-cov gcov
CCOVFLAGS = -g -O0 -fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage
# --coverage
COVFLAGS = -af

CC = clang++
CFLAGS = -std=c++2a -Wall -Wextra -pedantic -g

HEADERS = ../intrusive/pair.hpp ../intrusive/pair_fwd.hpp kvpq.hpp kvpq_fwd.hpp

all: tests

test: clean.cov all
	./tests

tests: tests_main.o tests_kvpq.o tests_load_factor.o
	$(CC) $(CFLAGS) $(CCOVFLAGS) $^ -o $@

tests_main.o: tests_main.cpp
	$(CC) $(CFLAGS) $< -c

tests_load_factor.o: tests_load_factor.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(CCOVFLAGS) -Wno-keyword-macro $< -c

tests_%.o: tests_%.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(CCOVFLAGS) $< -c

clean: clean.cov
	rm -f tests *.o

clean.cov:
	rm -f  *.gcov *.gcda *.gcno

cov: kvpq.hpp.gcov

kvpq.hpp.gcov: test
	$(COV) $(COVFLAGS) tests_kvpq.cpp
