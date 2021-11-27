/**
 * @file semantics.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @author Krystof Albrecht <xalbre05@stud.fit.vutbr.cz>
 * @brief Semantic functions mostly for type checks.
 */

#include "semantics.h"

static char *token_type_to_string(token_type type)
{
    switch (type) {
    case TOKEN_COLON:
        return ":";
    case TOKEN_LEFT_BRACKET:
        return "(";
    case TOKEN_RIGHT_BRACKET:
        return ")";
    case TOKEN_DECLAR:
        return "=";
    case TOKEN_EQUAL:
        return "==";
    case TOKEN_NOT_EQUAL_TO:
        return "~=";
    case TOKEN_LESS_THAN:
        return "<";
    case TOKEN_LESS_EQUAL_THAN:
        return "<=";
    case TOKEN_GREATER_THAN:
        return ">";
    case TOKEN_GREATER_EQUAL_THAN:
        return ">=";
    case TOKEN_ADD:
        return "+";
    case TOKEN_SUB:
        return "-";
    case TOKEN_MUL:
        return "*";
    case TOKEN_DIVISION:
        return "/";
    case TOKEN_FLOOR_DIVISION:
        return "//";
    case TOKEN_STRING_CONCAT:
        return "..";
    case TOKEN_STRING_LENGTH:
        return "#";
    case TOKEN_COMMA:
        return ",";
    case TOKEN_ID:
        return "identifier";
    case TOKEN_KEYWORD:
        return "keyword";
    case TOKEN_INT:
        return "integer literal";
    case TOKEN_NUMBER:
        return "number literal";
    case TOKEN_STRING:
        return "string literal";
    case TOKEN_EOF:
        return "end of file";
    case TOKEN_NON_TERMINAL:
    case TOKEN_HANDLE:
        return "";
    }

    return "";
}

void print_unexpected_token(T_token *bad_token, token_type expected_type, char *expected_content)
{
    char *unexpected_string = (*bad_token->value->content) == '\0'
        ? token_type_to_string(bad_token->type)
        : bad_token->value->content;
    if (*expected_content == '\0') {
        fprintf(stderr, "Error on line %d: Got unexpected token \"%s\", expected token %s. \n",
            bad_token->line, unexpected_string, token_type_to_string(expected_type));
    } else {
        fprintf(stderr, "Error on line %d: Got unexpected token \"%s\", expected %s %s. \n",
            bad_token->line, unexpected_string, token_type_to_string(expected_type),
            expected_content);
    }
}

bool token_list_type_assignable(tstack_s *first, tstack_s *second)
{
    sll_activate(first);
    sll_activate(second);

    while (sll_is_active(first) && sll_is_active(second)) {
        T_token *t1 = sll_get_active(first);
        T_token *t2 = sll_get_active(second);

        sll_next(first);
        sll_next(second);

        if (t1->symbol_type == SYM_TYPE_NUMBER && t2->symbol_type == SYM_TYPE_INT) {
            continue;
        }

        if (t1->symbol_type != t2->symbol_type) {
            return false;
        }
    }

    /* Either their length was the same or first was shorter therefore is no longer active */
    return (sll_is_active(first) == sll_is_active(second)) || !sll_is_active(first);
}

/**
 * @brief Checks if two stacks are type compatible. This include the fact that integer is a subset
 *of number.
 *
 * @param[in] first First stack.
 * @param[in] second Second stack.
 *
 * @return true All tokens have compatible types.
 * @return false Some tokens don't have compatible type.
 */
static bool token_list_types_compatible(tstack_s *first, tstack_s *second)
{
    sll_activate(first);
    sll_activate(second);

    while (sll_is_active(first) && sll_is_active(second)) {
        T_token *t1 = sll_get_active(first);
        T_token *t2 = sll_get_active(second);

        sll_next(first);
        sll_next(second);

        if (t1->symbol_type == SYM_TYPE_NUMBER && t2->symbol_type == SYM_TYPE_INT) {
            continue;
        }

        if (t1->symbol_type != t2->symbol_type) {
            return false;
        }
    }

    return sll_is_active(first) == sll_is_active(second);
}

bool sem_call_types_compatible(T_token *function, tstack_s *call_params, rc_e *rc)
{
    if (!token_list_types_compatible(function->fun_info->in_params, call_params)) {
        ERR_MSG("Function call has invalid parameters: ", function->line);
        fprintf(stderr, "%s\n", function->value->content);
        *rc = RC_SEM_CALL_ERR;
        return false;
    }

    return true;
}

bool token_list_types_identical(tstack_s *first, tstack_s *second)
{
    sll_activate(first);
    sll_activate(second);

    while (sll_is_active(first) && sll_is_active(second)) {
        T_token *t1 = sll_get_active(first);
        T_token *t2 = sll_get_active(second);

        if (t1->symbol_type != t2->symbol_type) {
            return false;
        }

        sll_next(first);
        sll_next(second);
    }

    return sll_is_active(first) == sll_is_active(second);
}
