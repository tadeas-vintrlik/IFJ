/**
 * @file exp_parser.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of bottom-up parser for expressions.
 */

#include "exp_parser.h"

#define TABLE_ELEM 10

/**
 * @brief Enumeration of different values found inside the precendece table.
 */
typedef enum op {
    /* Table heading */
    LEN,
    MULT_DIV,
    ADD_SUB,
    CONCAT,
    REL,
    LPAR,
    RPAR,
    ID,
    DOLLAR,
    /* Actions inside the table */
    PUSH, /* < */
    RULE, /* > */
    SPEC, /* = */
    ERR, /* x */
    NONE /* Empty table slot */
} op_e;

/**
 * @brief Precendence table.
 */
op_e table[TABLE_ELEM][TABLE_ELEM] = {
    { NONE, LEN, MULT_DIV, ADD_SUB, CONCAT, REL, LPAR, RPAR, ID, DOLLAR },
    { LEN, RULE, RULE, RULE, RULE, RULE, PUSH, ERR, PUSH, ERR },
    { MULT_DIV, PUSH, RULE, RULE, RULE, RULE, PUSH, RULE, PUSH, RULE },
    { ADD_SUB, PUSH, PUSH, RULE, RULE, RULE, PUSH, RULE, PUSH, RULE },
    { CONCAT, PUSH, PUSH, PUSH, PUSH, RULE, PUSH, RULE, PUSH, RULE },
    { REL, PUSH, PUSH, PUSH, PUSH, ERR, PUSH, RULE, PUSH, RULE },
    { LPAR, PUSH, PUSH, PUSH, PUSH, PUSH, PUSH, SPEC, PUSH, ERR },
    { RPAR, ERR, RULE, RULE, RULE, RULE, ERR, RULE, ERR, RULE },
    { ID, ERR, RULE, RULE, RULE, RULE, PUSH, RULE, ERR, RULE },
    { DOLLAR, ERR, PUSH, PUSH, PUSH, PUSH, PUSH, ERR, PUSH, ERR },
};

/**
 * @brief Returns op_e value of  token op_e searchable in the precedence table.
 *
 * @param token The token get op_e value.
 *
 * @return op_e value.
 */
static op_e token2op(const T_token *token)
{
    switch (token->type) {
    case TOKEN_STRING_LENGTH:
        return LEN;

    case TOKEN_MUL:
        return MULT_DIV;
    case TOKEN_DIVISION:
        return MULT_DIV;
    case TOKEN_FLOOR_DIVISION:
        return MULT_DIV;

    case TOKEN_ADD:
        return ADD_SUB;
    case TOKEN_SUB:
        return ADD_SUB;

    case TOKEN_STRING_CONCAT:
        return CONCAT;

    case TOKEN_LESS_THAN:
        return REL;
    case TOKEN_LESS_EQUAL_THAN:
        return REL;
    case TOKEN_GREATER_THAN:
        return REL;
    case TOKEN_GREATER_EQUAL_THAN:
        return REL;
    case TOKEN_EQUAL:
        return REL;
    case TOKEN_NOT_EQUAL_TO:
        return REL;

    case TOKEN_LEFT_BRACKET:
        return LPAR;

    case TOKEN_RIGHT_BRACKET:
        return RPAR;

    case TOKEN_ID:
        return ID;
    case TOKEN_INT:
        return ID;
    case TOKEN_NUMBER:
        return ID;
    case TOKEN_STRING:
        return ID;

    default:
        return DOLLAR;
    }
}

/**
 * @brief Get the right value from the precendece table.
 *
 * @param[in] in Terminal from input. Used as a key for columns.
 * @param[in] stack Terminal found on top of stack. Used as a key for rows.
 *
 * @return Action found in the table. NONE if @p in or @p stack was nout found as a key.
 */
op_e table_get_action(op_e in, op_e stack)
{
    op_e ret = NONE;
    int row = -1, col = -1;

    /* First find they key in columns */
    for (unsigned i = 0; i < TABLE_ELEM; i++) {
        if (table[0][in] == in) {
            col = i;
        }
    }

    /* Then find the key in rows */
    for (unsigned i = 0; i < TABLE_ELEM; i++) {
        if (table[i][0] == stack) {
            row = i;
        }
    }

    if (row != -1 && col != -1) {
        ret = table[row][col];
    }

    return ret;
}

avl_node_s *exp_parse(symtable_s *symtable)
{
    avl_node_s *node;
    avl_init(&node);

    (void)token2op;
    (void)symtable;
    return node;
}