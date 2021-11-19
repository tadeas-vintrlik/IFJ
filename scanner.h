/**
 * @file scanner.h
 * @author Josef Škorpík <xskorp07@stud.fit.vutbr.cz>
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @brief Interface for scanner.
 */

#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "common.h"
#include "token_stack.h"

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

#endif
