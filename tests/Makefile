CC=gcc
CFLAGS=-std=c99 -Wall -Werror -Wextra -pedantic
LFLAGS=-lcmocka
OBJ=../common.o
TESTS=test_common

.PHONY: all setup clean format run

all: setup $(TESTS)
run: setup $(TESTS)
	./test*

# For creating object files of units under test
setup:
	make -C ../

# Gerenic rule for creating tests
%: %.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@  -I../ $(LFLAGS)

# Formatting source files
format:
	clang-format -style="{BasedOnStyle: WebKit, PointerAlignment: Right, ColumnLimit: 100}" -i *.c

# Cleaning binaries
clean:
	rm -rf $(TESTS)
