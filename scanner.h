/**
 * @file scanner.h
 * @author Josef Škorpík <xskorp07@stud.fit.vutbr.cz>
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @brief Interface for scanner.
 */

#include "common.h"

#ifndef _SCANNER_H_
#define _SCANNER_H_

/**
 * @brief Enum for different types of tokens.
 */
typedef enum {
    TOKEN_ID,
    TOKEN_KEYWORD,
    TOKEN_INT,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_EQUAL,
    TOKEN_DECLAR,
    TOKEN_LESS_THAN,
    TOKEN_LESS_EQUAL_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_GREATER_EQUAL_THAN,
    TOKEN_DIVISION,
    TOKEN_FLOOR_DIVISION,
    TOKEN_MUL,
    TOKEN_SUB,
    TOKEN_NOT_EQUAL_TO,
    TOKEN_ADD,
    TOKEN_STRING_LENGTH,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_EOF
} token_type;

/**
 * @brief Enum for all the keywords.
 */
typedef enum {
    DO,
    ELSE,
    END,
    FUNCTION,
    GLOBAL,
    IF,
    INTEGER,
    LOCAL,
    NIL,
    NUMBER,
    REQUIRE,
    RETURN,
    STRING,
    THEN,
    WHILE
} Keyword;

/**
 * @brief Structure for storing a token
 */
typedef struct Token {
    token_type type;
    dynamic_string_s *value;
    int line;
} T_token;

/**
<<<<<<< HEAD
 * @brief Main function of scanner, returns a token
 * as a structure, returns an error code.
 *
 * @param[in/out] token Token structure for storing information about tokens.
 *
 * @return int Error value.
 */
int get_next_token(T_token *token);

/**
 * @brief Ungets a token, so that get_next_token returns it next time it is called.
 *
 * @param[in] token Token to unget.
 *
 * @return int Error code.
 */
int unget_token(T_token *token);

/**
 * @brief A desctructor for the token type. Frees all allocated memory.
 *
 * @param token The token to free.
 */
void token_destroy(T_token *token);

#endif
