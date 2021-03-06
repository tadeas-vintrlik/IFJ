/**
 * @file exp_parser.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of bottom-up parser for expressions.
 */

#include "exp_parser.h"
#include "token_stack.h"

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
 * @brief Precedence table.
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
 * @param[in] type The type of the non-terminal.
 * @param[in] line The line where the terminal from which this was created was found.
 *
 * @return Newly allocated non-terminal token.
 */
static T_token *create_non_terminal(symbol_type_e type, unsigned line)
{
    T_token *expr = malloc(sizeof *expr);
    ALLOC_CHECK(expr);

    token_init(expr);

    expr->type = TOKEN_NON_TERMINAL;
    expr->symbol_type = type;
    expr->line = line;
    return expr;
}

/**
 * @brief Get the right value from the precendece table.
 *
 * @param[in] in Terminal from input. Used as a key for columns.
 * @param[in] stack Terminal found on top of stack. Used as a key for rows.
 *
 * @return Action found in the table. NONE if @p in or @p stack was not found as a key.
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
        tstack_destroy(help);
        return false;
    }
    gen_expr_operand(terminal);
    non_terminal = create_non_terminal(terminal->symbol_type, terminal->line);
    if (!strcmp(terminal->value->content, "nil")) {
        non_terminal->symbol_type = SYM_TYPE_NIL;
    }
    tstack_push(tstack, non_terminal);
    token_destroy(terminal);
    return true;
}

/**
 * @brief Reduce non-terminal expression into atomic expression.
 *
 * @param[out] tstack Stack into which to insert the new expression.
 * @param[in] help Temporary stack to reduce using rules.
 * @param[out] rc Return code to set.
 */
static bool nonterm2expr(tstack_s *tstack, tstack_s *help, rc_e *rc)
{
    T_token *first, *second, *third;
    (void)first;

    /* Pop all three expected tokens */
    first = tstack_top(help);
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
        if (!sem_check_expr_type(first, second, third, rc)) {
            return false;
        }
        gen_expr_operator(second);
        token_destroy(second);
        break;

    default:
        token_destroy(second);
        return false;
    }

    token_destroy(first);
    tstack_push(tstack, third); /* Push back one reduced expression */

    return true;
}

/**
 * @brief Choose an expression rule to apply and modify the stack.
 *
 * @param[in,out] tstack Stack of tokens of expression to reduce using rules.
 * @param[out] rc Return code to set in case of an error.
 */
static bool apply_rule(tstack_s *tstack, rc_e *rc)
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
        gen_expr_operator(tmp);
        tstack_pop(&help, false);
        tmp = tstack_top(&help);
        if (tmp->type != TOKEN_NON_TERMINAL) {
            return false;
        }
        tstack_pop(&help, false);
        if (!sem_check_string_length(tmp, rc)) {
            return false;
        }
        tmp->symbol_type = SYM_TYPE_INT;
        tstack_push(tstack, tmp);
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
    case TOKEN_NUMBER:
    case TOKEN_INT:
    case TOKEN_STRING:
        if (!term2expr(tstack, &help)) {
            return false;
        }
        break;
    case TOKEN_NON_TERMINAL:
        if (!nonterm2expr(tstack, &help, rc)) {
            return false;
        }
        break;

    default:
        return false;
    }
    return true;
}

bool exp_parse(symtable_s *symtable, rc_e *rc, symbol_type_e *type)
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
            if (!sem_check_id_decl(token, symtable, &out, rc)) {
                return false;
            }
            /* Pass the type of the variable to local token for semantic checks */
            token->symbol_type = out->symbol_type;
        }
        switch (action) {
        case RULE:
            if (!apply_rule(&tstack, rc)) {
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
    while (apply_rule(&tstack, rc))
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
        ERR_MSG("Could not parse the entirety of the expression.\n", out->line);
        return false;
    }

    if (type) {
        *type = out->symbol_type;
    }
    token_destroy(out);
    tstack_destroy(&tstack);
    unget_token(token); /* Return the last unparsed token to top-down parser */
    return true;
}
