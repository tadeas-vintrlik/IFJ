CC=gcc
CFLAGS=-std=c99 -Wall -Werror -Wextra -pedantic
LFLAGS=-lcmocka
OBJ=../common.o
TESTS=test_common

.PHONY: all setup clean

all: setup $(TESTS)

# For creating object files of units under test
setup:
	make -C ../

# For creating tests
%: %.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS) -I../

clean:
	rm -rf $(TESTS)
