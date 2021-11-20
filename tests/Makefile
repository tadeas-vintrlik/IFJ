CC=gcc
CFLAGS=-std=c99 -g -Wall -Werror -Wextra -pedantic -I/usr/local/include
LFLAGS=-L/usr/local/lib -lcmocka
OBJ=../common.o ../sll.o ../scanner.o ../avl.o ../symtable.o ../token_stack.o
TESTS=test_common test_sll sc_tests/test_scanner1 sc_tests/test_scanner2 sc_tests/test_scanner3 sc_tests/test_scanner4 sc_tests/test_scanner5 test_avl test_symtable test_token_stack

.PHONY: all setup clean format run

all: setup $(TESTS)
run: setup $(TESTS)
	./test_run.sh

# For creating object files of units under test
setup:
	make debug -C ../

# Gerenic rule for creating tests
%: %.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@  -I../ $(LFLAGS)

# Formatting source files
format:
	clang-format -style="{BasedOnStyle: WebKit, PointerAlignment: Right, ColumnLimit: 100}" -i *.c

# Cleaning binaries
clean:
	rm -rf $(TESTS) *.dSYM
