#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

bool test1()
{

    bool passed = true;
    int err_value;

    token_type rec_types[30];
    char *rec_values[30];
    int rec_lines[30];
    token_type exp_types[17] = {TOKEN_KEYWORD, TOKEN_STRING, TOKEN_KEYWORD, TOKEN_ID,
                                TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_STRING,
                                TOKEN_COMMA, TOKEN_STRING, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_ID, TOKEN_LEFT_BRACKET,
                                TOKEN_RIGHT_BRACKET, TOKEN_EOF};
    char *exp_values[17] = {"require", "ifj21", "function", "hlavni_program", "", "", "write", "",
                            "Hello from IFJ21", "", "\\n", "", "end", "hlavni_program", "", "", ""};
    int exp_lines[17] = {1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 5, 5, 5, 6};
    T_token token;
    int no_of_tokens;
    int no_of_exp_tokens = 17;
    ;
    for (no_of_tokens = 0;; no_of_tokens++)
    {

        err_value = get_next_token(&token);
        if (err_value != 0 || no_of_tokens >= no_of_exp_tokens)
        {
            printf("LEX ERROR");
            passed = false;
            return passed;
        }
        if (token.type == TOKEN_EOF)
        {
            break;
        }
        passed = token.type == exp_types[no_of_tokens];
        passed = !strcmp(token.value->content, exp_values[no_of_tokens]);
        passed = token.line == exp_lines[no_of_tokens];
        rec_lines[no_of_tokens] = token.line;
        rec_values[no_of_tokens] = token.value->content;
        rec_types[no_of_tokens] = token.type;
    }
    passed = token.type == exp_types[no_of_tokens];
    passed = !strcmp(token.value->content, exp_values[no_of_tokens]);
    passed = token.line == exp_lines[no_of_tokens];
    rec_lines[no_of_tokens] = token.line;
    rec_values[no_of_tokens] = token.value->content;
    rec_types[no_of_tokens] = token.type;
    if (passed)
    {
        printf("OK\n");
        return passed;
    }
    printf("\n");
    printf("Expected types: ");
    for (int j = 0; j < no_of_exp_tokens; j++)
    {
        printer(exp_types[j]);
    }
    printf("\n");
    printf("===================================================================\n");
    printf("Recieved types: ");
    for (int j = 0; j <= no_of_tokens; j++)
    {
        printer(rec_types[j]);
    }
    printf("\n");
    printf("===================================================================\n");
    printf("Expected values: ");
    for (int j = 0; j < no_of_exp_tokens; j++)
    {
        printf("%s ", exp_values[j]);
    }
    printf("\n");
    printf("===================================================================\n");
    printf("Recieved values: ");
    for (int j = 0; j <= no_of_tokens; j++)
    {
        printf("%s ", rec_values[j]);
    }
    printf("\n");
    printf("===================================================================\n");
    printf("Expected lines: ");
    for (int j = 0; j < no_of_exp_tokens; j++)
    {
        printf("%d ", exp_lines[j]);
    }
    printf("\n");
    printf("===================================================================\n");
    printf("Recieved lines: ");
    for (int j = 0; j <= no_of_tokens; j++)
    {
        printf("%d ", rec_lines[j]);
    }
    printf("\n");

    return true;
}
int main()
{
    test1();
    return 0;
}
