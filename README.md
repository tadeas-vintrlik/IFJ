# IFJ

IFJ Compiler project - compiles a subset of the Teal language (statically typed Lua) to IFJcode21.

## Git workflow
- Avoid direct pushes to the **main** and **devel** branches when possible.
- Create a branch based on **devel** and then create a Pull Request once done with the changes.
```
git checkout devel
git brach -c new
git checkout new
```
- A Pull Request **must** be reviewed by the file owner (the top-most author) before merging.
- Every commit should have proper format before pushing. See Below.

## Format
- Based on the `llvm` style
- Right-alligned pointers
- 100 line limit
- for local headers use `#include "common.h"` and for system headers use `#include <stdio.h>`
- Auto-formatting via `clang-format` in Makefile (`sudo apt install clang-format` in Ubuntu)

## Makefile
| Command  | Description                          |
| -------- | ------------------------------------ |
| all      | Creates the main executable          |
| clean    | Remove objects and executables       |
| test_run | Runs all the tests                   |
| debug    | Creates debug version of executables |
| format   | Formats all source files             |

## Adding modules
- When unsure please follow the example set by the common.c and common.h
1. Create a .c source file and .h header file (**make sure** the header is guarded by an `ifndef`)
2. Include the header file in the required files
3. Add it into the `OBJ` variable in Makefile

## Adding a test case
1. Create a `static void` and `void **` parameter function with `asserts`
2. Add the function name into the `tests[]` array in `main` at the bottom of the test file
3. For tests to work you need the test library (`sudo apt install libcmocka-dev` in Ubuntu)

## Adding a test file
1. Copy the `test_common.c`
2. Rewrite it as neccessary (add includes for new modules)
3. If using a new module add it to the test Makefile `OBJ` variable (**make sure** it begins ../)
4. Add the name of the test binary into the test Makefile `TESTS` variable
