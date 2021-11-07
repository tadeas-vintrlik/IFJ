#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

bool test4()
{
    int err_value;
    int no_of_tokens;
    T_token token;

    for (no_of_tokens = 0;; no_of_tokens++)
    {

        err_value = get_next_token(&token);
        printer(token.type);
        if (err_value != 0)
        {
            printf("LEX ERROR AT: %d\n", no_of_tokens);
            return false;
        }
        if (token.type == TOKEN_EOF)
        {
            printf("OK\n");
            return true;
        }
    }
}

int main(void)
{
    test4();
}
