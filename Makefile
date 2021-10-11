CC=gcc
CFLAGS=-std=c99 -Wall -Werror -Wextra -pedantic
LFLAGS=
MAIN=main
OBJ=$(MAIN).o common.o

.PHONY: all debug debug_cflags clean test test_clean

all: $(MAIN)

# For running tests
test: debug
	make -C ./tests

# For cleaning the test directory
test_clean:
	make clean -C ./tests

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

clean: test_clean
	rm -rf $(OBJ) $(MAIN)
