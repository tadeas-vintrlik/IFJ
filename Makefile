CC=gcc
CFLAGS=-std=c99 -Wall -Werror -Wextra -pedantic
LFLAGS=
MAIN=main
OBJ=$(MAIN).o common.o sll.o

.PHONY: all debug debug_cflags clean test test_clean test_run format

all: $(MAIN)

# For building tests
test: debug
	make -C ./tests

# For cleaning the test directory
test_clean:
	make clean -C ./tests

# Formatting test source files
test_format:
	make format -C ./tests

# Run all the tets
test_run:
	make run -C ./tests

# Debug build
debug: clean debug_cflags all
debug_cflags:
	$(eval CFLAGS+=-g)

# Building main executable
$(MAIN): $(OBJ)
	$(CC) $^ -o $@ $(LFLAGS)

# Generic rule for creating objects
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Formatting source files
format: test_format
	clang-format -style="{BasedOnStyle: WebKit, PointerAlignment: Right, ColumnLimit: 100}" -i *.c *.h

# Cleaning objects and binaries
clean: test_clean
	rm -rf $(OBJ) $(MAIN)
