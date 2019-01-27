CC = g++
CFLAGS = -std=c++2a -fconcepts -Wall -Wextra -pedantic -g

all: recursive_iterate

recursive_iterate: recursive_iterate.cpp
	$(CC) $(CFLAGS) recursive_iterate.cpp -o recursive_iterate

clean:
	rm -rf recursive_iterate

test: all
	./recursive_iterate
