/**
 * @file main.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Main file used for creating the executable.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int main(void)
{
    char *line = NULL;
    size_t read = 0;
    source_file_s source;

    sf_init(&source);

    while (getline(&line, &read, stdin) != EOF) {
        sf_add_line(&source, line);
        FREE(line);
    }
    FREE(line);

    for (unsigned i = 0; i < source.no_lines; i++) {
        printf("%s", source.line[i]);
    }

    sf_destroy(&source);

    return RC_OK;
}
