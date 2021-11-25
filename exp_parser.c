/**
 * @file exp_parser.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
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
    { LEN, RULE, RULE, RULE, RULE, RULE, PUSH, RULE, PUSH, RULE },
    { MULT_DIV, PUSH, RULE, RULE, RULE, RULE, PUSH, RULE, PUSH, RULE },
    { ADD_SUB, PUSH, PUSH, RULE, RULE, RULE, PUSH, RULE, PUSH, RULE },
    { CONCAT, PUSH, PUSH, PUSH, PUSH, RULE, PUSH, RULE, PUSH, RULE },
    { REL, PUSH, PUSH, PUSH, PUSH, ERR, PUSH, RULE, PUSH, RULE },
    { LPAR, PUSH, PUSH, PUSH, PUSH, PUSH, PUSH, SPEC, PUSH, ERR },
    { RPAR, ERR, RULE, RULE, RULE, RULE, ERR, RULE, ERR, RULE },
    { ID, ERR, RULE, RULE, RULE, RULE, PUSH, RULE, ERR, RULE },
    { DOLLAR, PUSH, PUSH, PUSH, PUSH, PUSH, PUSH, ERR, PUSH, ERR },
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
    case TOKEN_KEYWORD:
        return ID; /* for nil */

    default:
        return DOLLAR;
    }
}

/**
 * @brief Create a non-terminal token.
 *
 * @return Newly allocated non-terminal token.
 */
static T_token *create_non_terminal(unsigned line)
{
    T_token *expr = malloc(sizeof *expr);
    ALLOC_CHECK(expr);
    expr->type = TOKEN_NON_TERMINAL;
    expr->value = NULL;
    expr->line = line;
    return expr;
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
    op_top = token2op(tstack_terminal_top(stack));
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
    T_token *non_terminal, *terminal = tstack_top(help);
    tstack_pop(help, false);

    if (!tstack_empty(help)) {
        // TODO: possibly free stack
        return false;
    }
    non_terminal = create_non_terminal(terminal->line);
    tstack_push(tstack, non_terminal);
    gen_expr_operand(terminal);
    token_destroy(terminal);
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
    first = tstack_top(help);
    tstack_pop(help, false);
    token_destroy(first);
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
        gen_expr_operator(second);
        token_destroy(second);
        break;

    default:
        token_destroy(second);
        return false;
    }

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
    T_token *tmp, *expr, *returnable;

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
    if (!tmp) {
        return false;
    }
    if (tmp->type != TOKEN_HANDLE) {
        /* Return it in case it was just one non-terminal */
        tstack_push(tstack, tmp);
        return false;
    }
    tstack_pop(&help, false); /* Remove the explicit handle */
    tmp = tstack_top(&help);

    /* Choose the rule according to left-most token */
    switch (tmp->type) {
    case TOKEN_STRING_LENGTH:
        tstack_pop(&help, false);
        tmp = tstack_top(&help);
        if (tmp->type != TOKEN_NON_TERMINAL) {
            return false;
        }
        tstack_pop(&help, false);
        tstack_push(tstack, tmp);
        /* TODO: Semantics for type of tmp */
        // TODO: code gen call
        break;
    case TOKEN_LEFT_BRACKET:
        returnable = tmp;
        tstack_pop(&help, false);
        tmp = tstack_top(&help);
        if (tmp->type != TOKEN_NON_TERMINAL) {
            return false;
        }
        expr = tmp; /* Store the inside expression to put as result */
        tstack_pop(&help, false);
        tmp = tstack_top(&help);
        if (!tmp || tmp->type != TOKEN_RIGHT_BRACKET) {
            tstack_push(tstack, returnable);
            tstack_push(tstack, expr);
            return false;
        }
        tstack_pop(&help, false);
        tstack_push(tstack, expr);
        break;
    case TOKEN_KEYWORD:
        /* Nil is the only valid keyword in expression */
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
        if (!nonterm2expr(tstack, &help)) {
            return false;
        }
        break;

    default:
        return false;
    }
    return true;
}

bool exp_parse(symtable_s *symtable, rc_e *rc)
{
    T_token *token, *out;
    op_e op, action;
    tstack_s tstack;
    tstack_init(&tstack);
    bool end = false;

    while (!end) {
        token = get_next_token();
        op = token2op(token);
        action = table_get_action(op, &tstack);
        if (action != ERR && token->type == TOKEN_ID) {
            /* If an identifier and part of the expression */
            if (!symtable_search_all(symtable, token->value->content, NULL)) {
                /* TODO: Same as error in parser rule_ARG make into a single function? */
                ERR_MSG("Use of undeclared variable: ", token->line);
                fprintf(stderr, "'%s'\n", token->value->content);
                *rc = RC_SEM_UNDEF_ERR;
                return false;
            }
        }
        switch (action) {
        case RULE:
            if (!apply_rule(&tstack)) {
                /* TODO: Improve the error message */
                ERR_MSG("Invalid expression.", token->line);
                return false;
            }
            unget_token(token);
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
    }

    /* Try to reduce it to one non-terminal if possible */
    while (apply_rule(&tstack))
        ;

    /* There should by only one non-terminal on stack */
    out = tstack_top(&tstack);
    if (!out) {
        ERR_MSG("Expected a non-empty expression.\n", token->line);
        /* Very first token was invalid */
        return false;
    }

    if (out->type == TOKEN_ID) {
        /* We assume the identifier was not a part of the expression as it might be a function call
         */
        unget_token(out);
        tstack_pop(&tstack, false);
        out = tstack_top(&tstack);
    }

    if (out->type != TOKEN_NON_TERMINAL) {
        ERR_MSG("Could not parse the expression: ", token->line);
        if (token->value && strlen(token->value->content)) {
            fprintf(stderr, "%s", token->value->content);
        } else {
            err_token_printer(token->type);
        }
        fprintf(stderr, " unexpected.\n");
        return false;
    }
    tstack_pop(&tstack, false);
    if (!tstack_empty(&tstack)) {
        /* TODO: Improve error message */
        ERR_MSG("Could not parse the entirety of the expression.\n", out->line);
        return false;
    }

    tstack_destroy(&tstack);
    unget_token(token); /* Return the last unparsed token to top-down parser */
    return true;
}
