#ifndef _COMMON_H_
#define _COMMON_H_

/**
 * @brief Enum for different types of tokens.
 */
typedef enum { ID, KEYWORD, DOUBLE } token_type;

/**
 * @brief Structure for storing a token
 */
typedef struct Token {
    token_type type;
    char *value;
    int line;
} * T_token;
/**
 * @brief Main function of scanner, returns a token
 * as a structure, returns a error code TODO
 *
 * @param[in/out] token Token structure for storing information about tokens.
 *
 * @return int Error value TODO.
 */
int get_next_token(T_token *token);
#endif
