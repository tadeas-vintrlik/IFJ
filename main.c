/**
 * @file main.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Main file used for creating the executable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int main(void)
{
    int ch = '\0';
    dynamic_string_s source;

    ds_init(&source);

    while (ch != EOF) {
        ch = getc(stdin);
        ds_add_char(&source, ch);
    }

    for (unsigned i = 0; i < source.size; i++) {
        printf("%c", source.content[i]);
    }
    ds_destroy(&source);

    return RC_OK;
}
