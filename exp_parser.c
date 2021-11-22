/**
 * @file exp_parser.c
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
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
    if (!token) {
        return DOLLAR;
    }
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
 * @brief Create a non-terminal token.
 *
 * @return Newly allocated non-terminal token.
 */
static T_token *create_non_terminal(void)
{
    T_token *handle = malloc(sizeof *handle);
    ALLOC_CHECK(handle);
    handle->type = TOKEN_NON_TERMINAL;
    handle->value = NULL;
    handle->line = -1;
    return handle;
}

/**
 * @brief Get the right value from the precendece table.
 *
 * @param[in] in Terminal from input. Used as a key for columns.
 * @param[in] stack Terminal found on top of stack. Used as a key for rows.
 *
 * @return Action found in the table. NONE if @p in or @p stack was nout found as a key.
 */
op_e table_get_action(op_e in, tstack_s *stack)
{
    op_e ret = NONE, op_top;
    int row = -1, col = -1;

    /* First find they key in columns */
    for (unsigned i = 0; i < TABLE_ELEM; i++) {
        if (table[0][i] == in) {
            col = i;
        }
    }

    /* Then find the key in rows */
    op_top = token2op(tstack_top(stack));
    for (unsigned i = 0; i < TABLE_ELEM; i++) {
        if (table[i][0] == op_top) {
            row = i;
        }
    }

    if (row != -1 && col != -1) {
        ret = table[row][col];
    }

    return ret;
}

/**
 * @brief Convert terminal to expression.
 *
 * @param[out] tstack Stack into which to insert the new expression.
 * @param[in] help Temporary stack to reduce using rules.
 */
static bool term2expr(tstack_s *tstack, tstack_s *help)
{
    T_token *non_terminal;
    T_token *terminal = tstack_top(help);
    tstack_pop(help, false);

    if (!tstack_empty(help)) {
        // TODO: possibly free stack
        return false;
    }
    non_terminal = create_non_terminal();
    tstack_push(tstack, non_terminal);
    // TODO: CODE_GEN call
    (void)terminal;
    return true;
}

/**
 * @brief Reduce non-terminal expression into atomic expression.
 *
 * @param[out] tstack Stack into which to insert the new expression.
 * @param[in] help Temporary stack to reduce using rules.
 */
static bool nonterm2expr(tstack_s *tstack, tstack_s *help)
{
    T_token *first, *second, *third;
    (void)first;

    /* Pop all three expected tokens */
    /* first = tstack_top(help); */
    tstack_pop(help, false);
    second = tstack_top(help);
    tstack_pop(help, false);
    third = tstack_top(help);
    tstack_pop(help, false);

    /* First was already checked in caller */
    if (!second || !third) {
        return false;
    }
    if (third->type != TOKEN_NON_TERMINAL) {
        return false;
    }
    if (!tstack_empty(help)) {
        return false;
    }

    switch (second->type) {
    /* Just need to check if valid operator */
    case TOKEN_MUL:
    case TOKEN_DIVISION:
    case TOKEN_FLOOR_DIVISION:
    case TOKEN_ADD:
    case TOKEN_SUB:
    case TOKEN_STRING_CONCAT:
    case TOKEN_LESS_THAN:
    case TOKEN_LESS_EQUAL_THAN:
    case TOKEN_GREATER_THAN:
    case TOKEN_GREATER_EQUAL_THAN:
    case TOKEN_EQUAL:
    case TOKEN_NOT_EQUAL_TO:
        break;

    default:
        token_destroy(second);
        return false;
    }

    /* When we get here, the expression is valid */
    /* TODO: Code gen call */
    tstack_push(tstack, third); /* Push back one reduced expression */

    return true;
}

/**
 * @brief Choose an expression rule to apply and modify the stack.
 *
 * @param[in,out] tstack Stack of tokens of expression to reduce using rules.
 */
static bool apply_rule(tstack_s *tstack)
{
    tstack_s help;
    T_token *tmp;

    tstack_init(&help);

    /* Get tokens until handle */
    while (!tstack_empty(tstack)) {
        tmp = tstack_top(tstack);

        tstack_pop(tstack, false);
        tstack_push(&help, tmp);
        if (tmp->type == TOKEN_HANDLE) {
            break;
        }
    }
    tmp = tstack_top(&help);

    /* If there were no tokens or no handle - syntax error */
    if (!tmp || (tmp && tmp->type != TOKEN_HANDLE)) {
        return false;
    }
    tstack_pop(&help, false); /* Remove the explicit handle */
    tmp = tstack_top(&help);

    /* Choose the rule according to left-most token */
    switch (tmp->type) {
    case TOKEN_STRING_LENGTH:
        tmp = tstack_top(&help);
        if (tmp->type != TOKEN_NON_TERMINAL) {
            return false;
        }
        tstack_pop(&help, false);
        // TODO: code gen call
        break;
    case TOKEN_LEFT_BRACKET:
        tmp = tstack_top(&help);
        if (tmp->type != TOKEN_NON_TERMINAL) {
            return false;
        }
        tstack_pop(&help, false);
        tmp = tstack_top(&help);
        if (tmp->type != TOKEN_RIGHT_BRACKET) {
            return false;
        }
        tstack_pop(&help, false);
        break;
    case TOKEN_KEYWORD:
        if (strcmp(tmp->value->content, "nil")) {
            return false;
        }
        if (!term2expr(tstack, &help)) {
            return false;
        }
        break;
    case TOKEN_ID:
        if (!term2expr(tstack, &help)) {
            return false;
        }
        break;
    case TOKEN_NUMBER:
        if (!term2expr(tstack, &help)) {
            return false;
        }
        break;
    case TOKEN_INT:
        if (!term2expr(tstack, &help)) {
            return false;
        }
        break;
    case TOKEN_STRING:
        if (!term2expr(tstack, &help)) {
            return false;
        }
        break;
    case TOKEN_NON_TERMINAL:
        tstack_push(&help, tmp); /* return the non-terminal needed to generate code */
        if (nonterm2expr(tstack, &help)) {
            return false;
        }
        break;

    default:
        return false;
    }
    return true;
}

bool exp_parse(symtable_s *symtable)
{
    T_token *token, *out;
    op_e op, action;
    tstack_s tstack;
    tstack_init(&tstack);
    bool end = false;

    while (!end) {
        token = get_next_token();
        if (token->type == TOKEN_ID) {
            if (!symtable_search_top(symtable, token->value->content, NULL)) {
                return false;
            }
        }

        op = token2op(token);
        action = table_get_action(op, &tstack);
        switch (action) {
        case RULE:
            apply_rule(&tstack);
            break;
        case PUSH:
            tstack_terminal_push(&tstack, token);
            break;
        case SPEC:
            tstack_push(&tstack, token);
            break;
        default:

            /* This is not necessarily an error, that is determined by the state of the stack */
            end = true;
            /* Should unget_token but that is handled later */
            break;
        }
        if (op == DOLLAR) {
            /* If input was dollar we asssume end of expression, should try to reduce the stack to
             * non-temrinals */
            unget_token(token);
        }
    }

    /* There should by only one non-terminal on stack */
    out = tstack_top(&tstack);
    if (!out) {
        /* Very first token was invalid */
        return false;
    }

    if (out->type != TOKEN_NON_TERMINAL) {
        return false;
    }
    tstack_pop(&tstack, false);
    if (!tstack_empty(&tstack)) {
        return false;
    }

    tstack_destroy(&tstack);
    return true;
}
