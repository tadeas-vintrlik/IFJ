
#include "scanner.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_next_token(T_token *token)
{
    (void)token;
    return 1;
    /* size_t lineSize = 0;
     char *line;
     size_t len = 0;
     lineSize = getline(&line, &len, stdin);
     if (lineSize == 0)
         return -1;
     (void)lineSize;
     free(line);
     return 0;*/
}
