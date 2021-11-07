#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

bool test5()
{

    bool passed = true;
    int err_value;

    token_type rec_types[50];
    char *rec_values[50];
    int rec_lines[50];
    token_type exp_types[] = {TOKEN_KEYWORD, TOKEN_STRING, TOKEN_KEYWORD, TOKEN_ID,
                              TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON,
                              TOKEN_KEYWORD, TOKEN_DECLAR, TOKEN_STRING, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_ID,
                              TOKEN_COMMA, TOKEN_STRING, TOKEN_RIGHT_BRACKET, TOKEN_ID, TOKEN_DECLAR, TOKEN_STRING,
                              TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_ID, TOKEN_COMMA, TOKEN_STRING, TOKEN_RIGHT_BRACKET,
                              TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON, TOKEN_KEYWORD, TOKEN_DECLAR, TOKEN_INT, TOKEN_SUB,
                              TOKEN_INT, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_ID, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD,
                              TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_EOF};
    char *exp_values[] = {"require", "ifj21", "function", "whitespaces", "", "", "local", "s", "",
                          "string", "", "\\x3A0", "write", "", "s", "", "\\n", "", "s", "", "a\\255b", "write", "",
                          "s", "", "\\n", "", "local", "x", "", "integer", "", "0", "", "1", "write", "", "x", "",
                          "end", "whitespaces", "", "", ""};
    int exp_lines[] = {1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 8, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 11, 11,
                       11, 12, 13, 13, 13, 13, 13, 13, 13, 13, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 17};
    T_token token;
    int no_of_tokens;
    int no_of_exp_tokens = 44;

    for (no_of_tokens = 0;; no_of_tokens++)
    {

        err_value = get_next_token(&token);
        if (err_value != 0 || no_of_tokens > no_of_exp_tokens)
        {
            printf("LEX ERROR at number: %d\n", no_of_tokens);
            passed = false;
            return passed;
        }
        if (token.type == TOKEN_EOF)
        {
            break;
        }
        passed = token.type == exp_types[no_of_tokens];
        passed = !strcmp(token.value->line, exp_values[no_of_tokens]);
        passed = token.line == exp_lines[no_of_tokens];

        rec_lines[no_of_tokens] = token.line;
        rec_values[no_of_tokens] = token.value->line;
        rec_types[no_of_tokens] = token.type;
        if (!passed)
        {
            printf("EXPECTED: ");
            printer(exp_types[no_of_tokens]);
            printf(" %s", exp_values[no_of_tokens]);
            printf("\n");
            printf("GOT: ");
            printer(token.type);
            printf(" %s", rec_values[no_of_tokens]);
            printf("\n");
        }
    }
    passed = token.type == exp_types[no_of_tokens];
    passed = !strcmp(token.value->line, exp_values[no_of_tokens]);
    passed = token.line == exp_lines[no_of_tokens];
    rec_lines[no_of_tokens] = token.line;
    rec_values[no_of_tokens] = token.value->line;
    rec_types[no_of_tokens] = token.type;
    if (passed)
    {
        printf("OK\n");
        return passed;
    }
    else
    {
        printf("TEST DIDNT PASS, VALUES BELOW");
    }
    printf("\n");
    printf("===================================================================\n");
    printf("\n");
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
        if (!strcmp(exp_values[j], ""))
        {
            continue;
        }
        printf("%s ", exp_values[j]);
    }
    printf("\n");
    printf("===================================================================\n");
    printf("Recieved values: ");
    for (int j = 0; j <= no_of_tokens; j++)
    {
        if (!strcmp(exp_values[j], ""))
        {
            continue;
        }
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
    test5();
    return 0;
}
