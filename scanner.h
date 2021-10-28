#ifndef _COMMON_H_
#define _COMMON_H_

typedef enum { ID, KEYWORD, DOUBLE } token_type;

typedef struct Token {
    token_type type;
    char *value;
    int line;
} * T_token;

#endif