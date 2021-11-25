/**
 * @file token_stack.h
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @author Josef Skorpik <xskorp07@stud.fit.vutbr.cz>
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Krystof Albrecht <xalbre05@stud.fit.vutbr.cz>
 * @brief Header for stack of tokens.
 */

#ifndef _TOKEN_STACK_H
#define _TOKEN_STACK_H

#include "common.h"
#include "sll.h"

/**
 * @brief Token stack structure.
 */
typedef sll_s tstack_s;

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
    TOKEN_STRING_CONCAT,
    TOKEN_ADD,
    TOKEN_STRING_LENGTH,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_EOF,
    TOKEN_HANDLE, /* Special token used for expression analysis */
    TOKEN_NON_TERMINAL, /* Special token used for expression analysis */
} token_type;

typedef enum {
    SYM_TYPE_STRING,
    SYM_TYPE_NUMBER,
    SYM_TYPE_INT,
    SYM_TYPE_NIL,
    SYM_TYPE_FUNCTION
} symbol_type_e;

typedef struct function_info {
    tstack_s *in_params;
    tstack_s *out_params;
    bool defined;
} function_info_s;

/**
 * @brief Structure for storing a token
 */
typedef struct Token {
    token_type type;
    dynamic_string_s *value;
    int line;

    symbol_type_e symbol_type;
    function_info_s *fun_info;
    bool declared; // TODO: Replace this with fun_info->defined
} T_token;

/**
 * @brief Initializes a token structure.
 *
 * @param token The token to initialize.
 */
void token_init(T_token *token);

/**
 * @brief A desctructor for the token type. Frees all allocated memory.
 *
 * @param token The token to free.
 */
void token_destroy(T_token *token);

/**
 * @brief Initialize the token stack structure.
 *
 * @param[in] tstack Token Stack to initialize.
 */
void tstack_init(tstack_s *tstack);

/**
 * @brief Insert a token into the Token Stack.
 *
 * @param[in] tstack Token Stack to push into.
 * @param[in] token Token to push.
 */
void tstack_push(tstack_s *tstack, T_token *token);

/**
 * @brief Get the token at the top of the stack.
 *
 * @param[in] tstack Token Stack to check top of.
 *
 * @return The token found on top. NULL if stack was empty.
 */
T_token *tstack_top(tstack_s *tstack);

/**
 * @brief Remove the top token.
 *
 * @param[in] tstack Token Stack to pop top of.
 * @param[in] destroy Whether or not to free the Token.
 */
void tstack_pop(tstack_s *tstack, bool destroy);

/**
 * @brief Get the token at the top of the stack.
 *
 * @param[in] tstack Token Stack to check top of.
 *
 * @return The top-most terminal token found. NULL if stack was empty.
 */
T_token *tstack_terminal_top(tstack_s *tstack);

/**
 * @brief Check if stack is empty.
 *
 * @param[in] tstack Token Stack to check for emptiness.
 *
 * @return If the token stack was empty.
 */
bool tstack_empty(const tstack_s *tstack);

/**
 * @brief Destructor for Token Stack..
 *
 * @param[in] tstack Token Stack to destory.
 */
void tstack_destroy(tstack_s *tstack);

/**
 * @brief Push down a terminal with handle. Handle will be pushed under the first terminal.
 *
 * @param[in] tstack Token Stack to push into.
 * @param[in] token Token to push.
 */
void tstack_terminal_push(tstack_s *tstack, T_token *token);

#endif /* _TOKEN_STACK_H */
