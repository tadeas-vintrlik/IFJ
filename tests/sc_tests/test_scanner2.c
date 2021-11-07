#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

bool test2()
{
    bool passed = true;
    int err_value;

    token_type rec_types[80];
    char *rec_values[82];
    int rec_lines[80];
    token_type exp_types[] = {TOKEN_KEYWORD, TOKEN_STRING, TOKEN_KEYWORD, TOKEN_ID,
                              TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON,
                              TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON, TOKEN_KEYWORD, TOKEN_DECLAR, TOKEN_INT,
                              TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_STRING, TOKEN_RIGHT_BRACKET, TOKEN_ID, TOKEN_DECLAR,
                              TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_ID, TOKEN_EQUAL,
                              TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_STRING,
                              TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID,
                              TOKEN_LESS_THAN, TOKEN_INT, TOKEN_KEYWORD, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_STRING,
                              TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_ID, TOKEN_DECLAR, TOKEN_INT, TOKEN_KEYWORD,
                              TOKEN_ID, TOKEN_GREATER_THAN, TOKEN_INT, TOKEN_KEYWORD, TOKEN_ID, TOKEN_DECLAR, TOKEN_ID,
                              TOKEN_MUL, TOKEN_ID, TOKEN_ID, TOKEN_DECLAR, TOKEN_ID, TOKEN_SUB, TOKEN_INT, TOKEN_KEYWORD,
                              TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_STRING, TOKEN_COMMA, TOKEN_ID, TOKEN_COMMA,
                              TOKEN_STRING, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID,
                              TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_EOF};
    char *exp_values[] = {"require", "ifj21", "function", "main", "", "", "local", "a", "",
                          "integer", "local", "vysl", "", "integer", "", "0", "write", "",
                          "Zadejte cislo pro vypocet faktorialu\\n", "", "a", "", "readi", "", "", "if", "a", "",
                          "nil", "then", "write", "", "a je nil\\n", "", "return", "else", "end", "if", "a", "", "0",
                          "then", "write", "", "Faktorial nelze spocitat\\n", "", "else", "vysl", "", "1", "while",
                          "a", "", "0", "do", "vysl", "", "vysl", "", "a", "a", "", "a", "", "1", "end", "write", "",
                          "Vysledek je: ", "", "vysl", "\\n", "", "end", "end", "main", "", "", ""};
    int exp_lines[] = {2, 2, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8,
                       9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 15, 16, 16,
                       16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 20, 20, 20, 20, 20, 20,
                       20, 20, 21, 22, 24, 24, 24, 25};
    T_token token;
    int no_of_tokens;
    int no_of_exp_tokens = 80;

    for (no_of_tokens = 0;; no_of_tokens++)
    {

        err_value = get_next_token(&token);
        if (err_value != 0)
        {
            printf("LEX ERROR at number: %d", no_of_tokens);
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
    test2();
    return 0;
}
