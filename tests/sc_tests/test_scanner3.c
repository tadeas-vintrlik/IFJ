#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

bool test2()
{

    bool passed = true;
    int err_value;

    token_type rec_types[120];
    char *rec_values[120];
    int rec_lines[120];
    token_type exp_types[] = {TOKEN_KEYWORD, TOKEN_STRING, TOKEN_KEYWORD, TOKEN_ID,
                              TOKEN_LEFT_BRACKET, TOKEN_ID, TOKEN_COLON, TOKEN_KEYWORD, TOKEN_RIGHT_BRACKET, TOKEN_COLON,
                              TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON, TOKEN_KEYWORD, TOKEN_DECLAR, TOKEN_ID,
                              TOKEN_SUB, TOKEN_INT, TOKEN_KEYWORD, TOKEN_ID, TOKEN_LESS_THAN, TOKEN_INT, TOKEN_KEYWORD,
                              TOKEN_KEYWORD, TOKEN_INT, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON,
                              TOKEN_KEYWORD, TOKEN_DECLAR, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_ID, TOKEN_RIGHT_BRACKET,
                              TOKEN_KEYWORD, TOKEN_ID, TOKEN_MUL, TOKEN_ID, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD,
                              TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_ID, TOKEN_LEFT_BRACKET,
                              TOKEN_STRING, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON, TOKEN_KEYWORD,
                              TOKEN_DECLAR, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_ID,
                              TOKEN_NOT_EQUAL_TO, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID, TOKEN_LESS_THAN,
                              TOKEN_INT, TOKEN_KEYWORD, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_STRING, TOKEN_COMMA,
                              TOKEN_STRING, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID, TOKEN_COLON,
                              TOKEN_KEYWORD, TOKEN_DECLAR, TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_ID, TOKEN_RIGHT_BRACKET,
                              TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_STRING, TOKEN_COMMA, TOKEN_ID, TOKEN_COMMA,
                              TOKEN_STRING, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_ID,
                              TOKEN_LEFT_BRACKET, TOKEN_STRING, TOKEN_RIGHT_BRACKET, TOKEN_KEYWORD, TOKEN_KEYWORD,
                              TOKEN_ID, TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_EOF};
    char *exp_values[] = {"require", "ifj21", "function", "factorial", "", "n", "", "integer", "",
                          "", "integer", "local", "n1", "", "integer", "", "n", "", "1", "if", "n", "", "2", "then",
                          "return", "1", "else", "local", "tmp", "", "integer", "", "factorial", "", "n1", "",
                          "return", "n", "", "tmp", "end", "end", "function", "main", "", "", "write", "",
                          "Zadejte cislo pro vypocet faktorialu: ", "", "local", "a", "", "integer", "", "readi", "",
                          "", "if", "a", "", "nil", "then", "if", "a", "", "0", "then", "write", "",
                          "Faktorial nejde spocitat!", "", "\\n", "", "else", "local", "vysl", "", "integer", "",
                          "factorial", "", "a", "", "write", "", "Vysledek je ", "", "vysl", "", "\\n", "", "end",
                          "else", "write", "", "Chyba pri nacitani celeho cisla!\\n", "", "end", "end", "main", "",
                          "", ""};
    int exp_lines[] = {2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7,
                       7, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 11, 12, 14, 14, 14, 14, 15, 15, 15, 15, 16,
                       16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19,
                       20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 24, 25, 25, 25,
                       25, 26, 27, 29, 29, 29, 29};
    T_token token;
    int no_of_tokens;
    int no_of_exp_tokens = 104;

    for (no_of_tokens = 0;; no_of_tokens++)
    {

        err_value = get_next_token(&token);
        if (err_value != 0)
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
        if (!passed)
            printf("ERROR AT INDEX: %d\n", no_of_tokens);
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
    else
    {
        printf("TEST DIDNT PASS, VALUES BELOW");
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
    test2();
    return 0;
}
