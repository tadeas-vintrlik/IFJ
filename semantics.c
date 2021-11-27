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

bool sem_check_call_function(T_token *token, symtable_s *symtable, T_token **function, rc_e *rc)
{
    if (!symtable_search_global(symtable, token->value->content, function)) {
        ERR_MSG("Use of undefined function: ", token->line);
        fprintf(stderr, "'%s'\n", token->value->content);
        *rc = RC_SEM_UNDEF_ERR;
        return false;
    }
    return true;
}
bool sem_check_redecl(T_token *token, symtable_s *symtable, rc_e *rc)
{
    T_token *original;
    if (symtable_search_global(symtable, token->value->content, &original)) {
        if (original->fun_info->defined) {
            ERR_MSG("Function declaration follows function definition: ", token->line);
        } else {
            ERR_MSG("Redeclaring already declared function: ", token->line);
        }

        if (original->line == -1) {
            fprintf(stderr, "'%s' is a built-in function.\n", token->value->content);
        } else {
            fprintf(stderr, "'%s' original on line: %d\n", token->value->content, original->line);
        }

        *rc = RC_SEM_UNDEF_ERR;
        return false;
    }

    return true;
}

bool sem_check_redef(T_token *token, symtable_s *symtable, T_token **function, rc_e *rc)
{
    if (symtable_search_global(symtable, token->value->content, function)) {
        if ((*function)->fun_info->defined) {
            /* If the function was already defined */
            ERR_MSG("Redefining function: ", token->line);
            if ((*function)->line == -1) {
                fprintf(stderr, "'%s' is a built-in function.\n", token->value->content);
            } else {
                fprintf(stderr, "'%s' original declaration on line: %d\n", token->value->content,
                    (*function)->line);
            }
            *rc = RC_SEM_UNDEF_ERR;
            return false;
        }
        return false;
    }
    return true;
}

static bool token_list_types_identical(tstack_s *first, tstack_s *second)
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

bool sem_check_decl_def_params(T_token *token, tstack_s *in_params, rc_e *rc)
{
    if (!token_list_types_identical(token->fun_info->in_params, in_params)) {
        ERR_MSG("Mismatch in definition and declaration parameter types.", token->line);
        *rc = RC_SEM_UNDEF_ERR;
        tstack_destroy(in_params);
        FREE(in_params);
        return false;
    }
    tstack_destroy(token->fun_info->in_params);
    FREE(token->fun_info->in_params);
    return true;
}

bool sem_check_decl_def_returns(T_token *token, tstack_s *out_params, rc_e *rc)
{
    if (!token_list_types_identical(token->fun_info->out_params, out_params)) {
        ERR_MSG("Mismatch in definition and declaration return types.", token->line);
        *rc = RC_SEM_UNDEF_ERR;
        tstack_destroy(out_params);
        FREE(out_params);
        return false;
    }
    tstack_destroy(token->fun_info->out_params);
    FREE(token->fun_info->out_params);
    return true;
}
