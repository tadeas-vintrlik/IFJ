#include <stdbool.h>
#include <stdio.h>

#include "scanner.h"
/*KEYWORD STRING KEYWORD INDETIFIER LEFT_PAR RIGHT_PAR KEYWORD LEFT_PAR STRING COMMA STRING
RIGHT_PAR KEYWORD INDETIFIER LEFT_PAR RIGHT_PAR*/

// DISCALIMER, messy code af, dont judge pls
bool test1()
{
    int no_of_tokens = 16;
    /* token_type array [] = {KEYWORD, STRING, KEYWORD, ID, LEFT_PAR, RIGHT_PAR, KEYWORD,
     LEFT_PAR, STRING, COMMA, STRING, RIGHT_PAR, KEYWORD, ID,LEFT_PAR, RIGHT_PAR};*/

    token_type recieved_types[16];
    char *rec_values[16];
    int rec_lines[16];
    for (int i = 0; i < no_of_tokens; i++) {
        T_token token;
        get_next_token(&token);
        recieved_types[i] = token.type;
        rec_values[i] = token.value;
        rec_lines[i] = token.line;
    }

    printf("Expected types: KEYWORD, STRING, KEYWORD, ID, LEFT_PAR, RIGHT_PAR, KEYWORD,"
           "LEFT_PAR, STRING, COMMA, STRING, RIGHT_PAR, KEYWORD, ID,LEFT_PAR, RIGHT_PAR");
    printf("\n");
    printf("Expected values: require \"ifj21\" function hlavni_program TRIVIAL TRIVIAL write"
           "TRIVIAL \"hello from IFJ21\" TRIVIAL \"\\n\" TRIVIAL"
           "end hlavni_program TRIVIAL TRIVIAL");
    printf("\n");
    printf("Expected lines. 1 1 2 2 2 2 3 3 3 3 3 3 4 5 5 5");
    printf("\n");
    printf("===================================================================\n");
    printf("Recieved types: ");
    for (int i = 0; i < no_of_tokens; i++) {
        printf("%x ", recieved_types[i]);
    }
    printf("\n");
    printf("===================================================================\n");
    printf("Recieved values: ");
    for (int i = 0; i < no_of_tokens; i++) {
        printf("%s ", rec_values[i]);
    }
    printf("\n");
    printf("===================================================================\n");
    printf("Recieved lines: ");
    for (int i = 0; i < no_of_tokens; i++) {
        printf("%d ", rec_lines[i]);
    }
    printf("\n");

    return true;
}
int main()
{
    test1();
    return 0;
}
