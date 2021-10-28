CC=gcc
CFLAGS=-std=c99 -Wall -Werror -Wextra -pedantic -g
LFLAGS=-lcmocka
OBJ=../common.o ../sll.o
TESTS=test_common test_sll

.PHONY: all setup clean format run

all: setup $(TESTS)
run: setup $(TESTS)
	./test_run.sh

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
