/**
 * @file parser.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Krystof Albrecht <xalbre05@stud.fit.vutbr.cz>
 * @author Josef Skorpik <xskorp07@stud.fit.vutbr.cz>
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Top-down parser using recursive descent.
 */

#include "parser.h"
#include "common.h"
#include "symtable.h"
#include "token_stack.h"
#include <stdbool.h>

static rc_e rc = RC_SYN_ERR;

static void print_unexpected_token(
    T_token *bad_token, token_type expected_type, char *expected_content);
static char *token_type_to_string(token_type token_type);

static bool token_list_types_identical(tstack_s *, tstack_s *);

#define GET_CHECK(TYPE)                                                                            \
    token = get_next_token();                                                                      \
    if (token->type != TYPE) {                                                                     \
        print_unexpected_token(token, TYPE, "");                                                   \
        return false;                                                                              \
    }

#define GET_CHECK_CMP(TYPE, VALUE)                                                                 \
    token = get_next_token();                                                                      \
    if (token->type != TYPE || strcmp(VALUE, token->value->content) != 0) {                        \
        print_unexpected_token(token, TYPE, VALUE);                                                \
        return false;                                                                              \
    }

static symtable_s symtable;

static bool rule_PROG();
static bool rule_CODE();
static bool rule_TOP_ELEM();

static bool rule_DECL();
static bool rule_DEF();
static bool rule_CALL();

static bool rule_PARAM_LIST(tstack_s *);
static bool rule_NEXT_PARAM(tstack_s *);
static bool rule_PARAM(tstack_s *);

static bool rule_RET_LIST(tstack_s *);
static bool rule_TYPE_LIST(tstack_s *);
static bool rule_NEXT_TYPE(tstack_s *);
static bool rule_TYPE(tstack_s *);

static bool rule_ARG_LIST();
static bool rule_ARG();
static bool rule_NEXT_ARG();

static bool rule_BODY();
static bool rule_STATEMENT_LIST();

static bool rule_IF_ELSE();
static bool rule_WHILE();
static bool rule_EXPR();
static bool rule_VAR_DECL();

static bool left_side_function();
static bool right_side_function(sll_s *left_side_ids);

rc_e start_parsing()
{
    rc_e ret;
    symtable_init(&symtable);
    ret = rule_PROG() ? RC_OK : rc;
    symtable_destroy(&symtable);
    return ret;
}

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

static void print_unexpected_token(
    T_token *bad_token, token_type expected_type, char *expected_content)
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

/**
 * @brief Checks if all tokens in two stacks of tokens have the same type. Will change the activity
 * of both lists.
 *
 * @param[in] first First stack.
 * @param[in] second Second stack.
 *
 * @return true All tokens have the same type.
 * @return false Some tokens don't have the same type.
 */
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

static bool rule_PROG()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "require");
    token_destroy(token);

    GET_CHECK_CMP(TOKEN_STRING, "ifj21");
    token_destroy(token);

    gen_prog_start();

    return rule_CODE();
}

static bool rule_CODE()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_EOF) {
        token_destroy(token);
        return true;
    } else {
        unget_token(token);
        return rule_TOP_ELEM() && rule_CODE();
    }
}

static bool rule_TOP_ELEM()
{
    T_token *first_token = get_next_token();

    if (first_token->type == TOKEN_ID) {
        /* Declaration of function checked in rule_CALL */
        unget_token(first_token);
        return rule_CALL();
    } else if (first_token->type == TOKEN_KEYWORD) {
        unget_token(first_token);
        if (!strcmp("global", first_token->value->content)) {
            return rule_DECL();
        } else if (!strcmp("function", first_token->value->content)) {
            return rule_DEF();
        }
    }

    print_unexpected_token(first_token, TOKEN_KEYWORD, "global or function or identifier");
    return false;
}

static bool rule_CALL()
{
    T_token *token, *function;
    /* TODO: in_params has to be freed */
    tstack_s *in_params = malloc(sizeof *in_params);
    ALLOC_CHECK(in_params);
    tstack_init(in_params);

    GET_CHECK(TOKEN_ID);
    if (!symtable_search_global(&symtable, token->value->content, &function)) {
        ERR_MSG("Use of undefined function: ", token->line);
        fprintf(stderr, "'%s'\n", token->value->content);
        rc = RC_SEM_UNDEF_ERR;
        return false;
    }
    token_destroy(token);

    GET_CHECK(TOKEN_LEFT_BRACKET);
    token_destroy(token);

    if (!rule_ARG_LIST(&in_params)) {
        return false;
    }

    /* TODO: code-gen gen_func_call */
    GET_CHECK(TOKEN_RIGHT_BRACKET);
    token_destroy(token);

    return true;
}

static bool rule_DECL()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "global");
    token_destroy(token);

    // TODO: Check for redeclaration (semantic)
    GET_CHECK(TOKEN_ID);
    token->fun_info = malloc(sizeof(function_info_s));
    ALLOC_CHECK(token->fun_info);
    function_info_init(token->fun_info);
    token->fun_info->defined = false;

    symtable_insert_token_global(&symtable, token);
    T_token *function_symbol = token;

    GET_CHECK(TOKEN_COLON);
    token_destroy(token);
    GET_CHECK_CMP(TOKEN_KEYWORD, "function");
    token_destroy(token);
    GET_CHECK(TOKEN_LEFT_BRACKET);
    token_destroy(token);

    if (!rule_TYPE_LIST(function_symbol->fun_info->in_params)) {
        return false;
    }

    GET_CHECK(TOKEN_RIGHT_BRACKET);
    token_destroy(token);

    return rule_RET_LIST(function_symbol->fun_info->out_params);
}

static bool rule_DEF()
{
    T_token *token, *original = NULL, *function_symbol = NULL;
    GET_CHECK_CMP(TOKEN_KEYWORD, "function");
    token_destroy(token);

    GET_CHECK(TOKEN_ID);

    tstack_s *declared_types_in = NULL;
    tstack_s *declared_types_out = NULL;

    if (symtable_search_global(&symtable, token->value->content, &original)) {
        if (original->fun_info->defined) {
            /* If the function was already defined */
            ERR_MSG("Redefining function: ", token->line);
            if (original->line == -1) {
                fprintf(stderr, "'%s' is a built-in function.\n", token->value->content);
            } else {
                fprintf(stderr, "'%s' original declaration on line: %d\n", token->value->content,
                    original->line);
            }
            rc = RC_SEM_UNDEF_ERR;
            return false;
        } else {
            /* If the function was NOT already defined, but WAS declared */
            /* TODO: Check if types of decalaration and definition match */
            original->fun_info->defined = true;

            declared_types_in = original->fun_info->in_params;
            declared_types_out = original->fun_info->out_params;
            function_symbol = original;
        }
    } else {
        token->fun_info = malloc(sizeof(function_info_s));
        ALLOC_CHECK(token->fun_info);
        function_info_init(token->fun_info);
        token->fun_info->defined = true;

        symtable_insert_token_global(&symtable, token);
        function_symbol = token;
    }

    /* TODO: code-gen gen_func_start and gen_pop_arg */

    GET_CHECK(TOKEN_LEFT_BRACKET);
    token_destroy(token);

    symtable_new_frame(&symtable);

    tstack_s *defined_params_in = malloc(sizeof(tstack_s));
    ALLOC_CHECK(defined_params_in);
    tstack_init(defined_params_in);

    tstack_s *defined_types_out = malloc(sizeof(tstack_s));
    ALLOC_CHECK(defined_types_out);
    tstack_init(defined_types_out);

    if (!rule_PARAM_LIST(defined_params_in)) {
        return false;
    }

    GET_CHECK(TOKEN_RIGHT_BRACKET);
    token_destroy(token);

    if (!rule_RET_LIST(defined_types_out)) {
        return false;
    }

    // HERE COMES THE FUN!

    if (declared_types_in) {
        if (!token_list_types_identical(declared_types_in, defined_params_in)) {
            ERR_MSG(
                "Mismatch in definition and declaration parameter types.", function_symbol->line);
            tstack_destroy(declared_types_in);
            FREE(declared_types_in);
            rc = RC_SEM_UNDEF_ERR;
            return false;
        }
        tstack_destroy(declared_types_in);
        FREE(declared_types_in);
    }

    if (declared_types_out) {
        if (!token_list_types_identical(declared_types_out, defined_types_out)) {
            ERR_MSG("Mismatch in definition and declaration return types.", function_symbol->line);
            tstack_destroy(declared_types_out);
            FREE(declared_types_out);
            rc = RC_SEM_UNDEF_ERR;
            return false;
        }
        tstack_destroy(declared_types_out);
        FREE(declared_types_out);
    }

    // Must copy in params as they are also on the local frame (so they don't get destroyed)
    tstack_s *copy = tstack_copy(defined_params_in);
    function_symbol->fun_info->in_params = copy;
    function_symbol->fun_info->out_params = defined_types_out;

    if (!rule_BODY()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    symtable_pop_frame(&symtable);
    /* TODO: code-gen gen_func_end */
    token_destroy(token);

    return true;
}

static bool rule_PARAM_LIST(tstack_s *collected_params)
{
    if (!rule_PARAM(collected_params)) {
        return true;
    }

    return rule_NEXT_PARAM(collected_params);
}

static bool rule_NEXT_PARAM(tstack_s *collected_params)
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COMMA) {
        token_destroy(token);
        // TODO Print unexpected token errors
        return rule_PARAM(collected_params) && rule_NEXT_PARAM(collected_params);
    } else {
        unget_token(token);
        return true;
    }
}

static bool rule_PARAM(tstack_s *collected_params)
{
    T_token *token = get_next_token();

    if (token->type != TOKEN_ID) {
        unget_token(token);
        return false;
    }

    symtable_insert_token_top(&symtable, token);
    sll_insert_last(collected_params, token);
    T_token *param_token = token;

    /* TODO: Add param type to function in global frame */
    /* <TODO: code-gen gen_param_caller>_in */

    GET_CHECK(TOKEN_COLON);
    token_destroy(token);

    if (!rule_TYPE(collected_params)) {
        return false;
    }

    if (collected_params) {
        T_token *type_token = sll_get_last(collected_params);
        sll_delete_last(collected_params, false);

        param_token->symbol_type = type_token->symbol_type;
        token_destroy(type_token);
    } else {
        token_destroy(param_token);
    }

    return true;
}

static bool rule_RET_LIST(tstack_s *collected_types)
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COLON) {
        token_destroy(token);
        return rule_TYPE(collected_types) && rule_NEXT_TYPE(collected_types);
    } else {
        unget_token(token);
        return true;
    }
}

static bool rule_TYPE_LIST(tstack_s *collected_types)
{
    if (!rule_TYPE(collected_types)) {
        return true;
    }

    return rule_NEXT_TYPE(collected_types);
}

static bool rule_NEXT_TYPE(tstack_s *collected_types)
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COMMA) {
        token_destroy(token);
        // TODO Print unexpected token errors
        return rule_TYPE(collected_types) && rule_NEXT_TYPE(collected_types);
    } else {
        unget_token(token);
        return true;
    }
}

static bool rule_TYPE(tstack_s *collected_types)
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_KEYWORD) {
        if (!strcmp("nil", token->value->content)) {
            token->symbol_type = SYM_TYPE_NIL;
        } else if (!strcmp("integer", token->value->content)) {
            token->symbol_type = SYM_TYPE_INT;
        } else if (!strcmp("number", token->value->content)) {
            token->symbol_type = SYM_TYPE_NUMBER;
        } else if (!strcmp("string", token->value->content)) {
            token->symbol_type = SYM_TYPE_STRING;
        } else {
            unget_token(token);
            return false;
        }

        if (collected_types) {
            // Using sll_s method on tstack_s
            sll_insert_last(collected_types, token);
        } else {
            token_destroy(token);
        }
        return true;
    }

    unget_token(token);
    return false;
}

static bool rule_ARG_LIST(tstack_s **in_params)
{
    if (!rule_ARG(in_params)) {
        return true;
    }

    return rule_NEXT_ARG(in_params);
}

static bool rule_ARG(tstack_s **in_params)
{
    T_token *token = get_next_token();

    /*
     * TODO: code-gen gen_push_ret
     * TODO: Add param to decide if called as return ARG_LIST or CALL(ARG_LIST)
     */
    switch (token->type) {
    case TOKEN_ID:
        if (!symtable_search_all(&symtable, token->value->content, NULL)) {
            ERR_MSG("Use of undeclared variable: ", token->line);
            fprintf(stderr, "'%s'\n", token->value->content);
            rc = RC_SEM_UNDEF_ERR;
            return false;
        }
        break;
    case TOKEN_NUMBER:
    case TOKEN_INT:
    case TOKEN_STRING:
        break;
    case TOKEN_KEYWORD:
        if (!strcmp("nil", token->value->content)) {
            break;
        }

        unget_token(token);
        return false;
    default:
        unget_token(token);
        return false;
    }

    sll_insert_last(*in_params, token);
    return true;
}

static bool rule_NEXT_ARG(tstack_s **in_params)
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COMMA) {
        token_destroy(token);

        // TODO Print unexpected token errors
        return rule_ARG(in_params) && rule_NEXT_ARG(in_params);
    } else {
        unget_token(token);
        return true;
    }
}

///////////

static bool rule_BODY()
{
    if (!rule_STATEMENT_LIST()) {
        return false;
    }
    return true;
}

static bool rule_STATEMENT_LIST()
{
    T_token *token = get_next_token();
    /* TODO: ret_list has to be freed */
    tstack_s *ret_list = malloc(sizeof *ret_list);
    ALLOC_CHECK(ret_list);
    tstack_init(ret_list);

    switch (token->type) {
    case TOKEN_KEYWORD:
        unget_token(token);

        if (!strcmp("return", token->value->content)) {
            token = get_next_token(); // Skip the token we returned to the scanner
            token_destroy(token);

            token = get_next_token();
            // TODO: Fix this shit
            switch (token->type) {
            case TOKEN_ID:
            case TOKEN_NUMBER:
            case TOKEN_INT:
            case TOKEN_STRING:
                unget_token(token);

                sll_s ids;
                sll_init(&ids);

                return right_side_function(&ids);
            case TOKEN_KEYWORD:
                if (!strcmp("nil", token->value->content)) {
                    sll_s ids;
                    sll_init(&ids);

                    unget_token(token);
                    return right_side_function(&ids);
                }

                unget_token(token);
                return true;
            default:
                unget_token(token);
                return true;
            }
            // TODO: Fix this shit
        } else if (!strcmp("if", token->value->content)) {
            return rule_IF_ELSE() && rule_STATEMENT_LIST();
        } else if (!strcmp("while", token->value->content)) {
            return rule_WHILE() && rule_STATEMENT_LIST();
        } else if (!strcmp("local", token->value->content)) {
            return rule_VAR_DECL() && rule_STATEMENT_LIST();
        }

        return true;
    case TOKEN_ID:
        unget_token(token);

        return left_side_function() && rule_STATEMENT_LIST();
    default:
        unget_token(token);
        return true;
    }
}

static bool rule_IF_ELSE()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "if");
    token_destroy(token);

    if (!rule_EXPR()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "then");
    /* TODO: code-gen gen_jump_else */
    token_destroy(token);

    symtable_new_frame(&symtable);
    if (!rule_BODY()) {
        return false;
    }
    symtable_pop_frame(&symtable);

    /* TODO: code-gen gen_jump_if_end */

    GET_CHECK_CMP(TOKEN_KEYWORD, "else");
    /* TODO: code-gen gen_else_label */
    token_destroy(token);

    symtable_new_frame(&symtable);
    if (!rule_BODY()) {
        return false;
    }
    symtable_pop_frame(&symtable);

    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    /* TODO: code-gen gen_if_end */
    token_destroy(token);

    return true;
}

static bool rule_WHILE()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "while");
    /* TODO: code-gen gen_while_label */
    token_destroy(token);

    if (!rule_EXPR()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "do");
    /* TODO: code-gen gen_jump_while_end */
    token_destroy(token);

    symtable_new_frame(&symtable);
    if (!rule_BODY()) {
        return false;
    }
    symtable_pop_frame(&symtable);

    /* TODO: code-gen gen_jump_while */

    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    /* TODO: code-gen gen_while_end */
    token_destroy(token);

    return true;
}

static bool rule_VAR_DECL()
{
    T_token *token;
    sll_s id;
    sll_init(&id);

    GET_CHECK_CMP(TOKEN_KEYWORD, "local");
    token_destroy(token);

    GET_CHECK(TOKEN_ID);
    if (symtable_search_top(&symtable, token->value->content, NULL)) {
        ERR_MSG("Redeclaring variable error: ", token->line)
        fprintf(stderr, "'%s'\n", token->value->content);
        rc = RC_SEM_UNDEF_ERR;
        return false;
    }
    if (symtable_search_global(&symtable, token->value->content, NULL)) {
        ERR_MSG("Redeclaring function name as variable error: ", token->line)
        fprintf(stderr, "'%s'\n", token->value->content);
        rc = RC_SEM_UNDEF_ERR;
        return false;
    }
    symtable_insert_token_top(&symtable, token);
    sll_insert_head(&id, token);

    GET_CHECK(TOKEN_COLON);
    token_destroy(token);

    // TODO: Collect the variable's type
    if (!rule_TYPE(NULL)) {
        return false;
    }

    token = get_next_token();

    if (token->type == TOKEN_DECLAR) {
        token_destroy(token);
        // TODO:Error on multiple expressions on right side
        return right_side_function(&id);
    } else {
        // TODO:Codegen assign nil to this new variable
        unget_token(token);
        return true;
    }
}

static bool rule_EXPR() { return exp_parse(&symtable, &rc); }

static bool left_side_function()
{
    T_token *token;
    sll_s left_side_ids;
    sll_init(&left_side_ids);

    GET_CHECK(TOKEN_ID);

    // We need two tokens to decide if this is a function call
    T_token *token2 = get_next_token();

    unget_token(token2);
    unget_token(token);

    // Assuming function call (no assignment, left_side_ids is empty)
    if (token2->type == TOKEN_LEFT_BRACKET) {
        return right_side_function(&left_side_ids);
    }

    while (true) {
        GET_CHECK(TOKEN_ID);

        // TODO:Change to sll_insert_last, right_side_function reads from left to right
        sll_insert_head(&left_side_ids, token);

        token = get_next_token();

        // We have collected all of the left side IDs
        if (token->type == TOKEN_DECLAR) {
            break;
        }

        // IDs must be separated by commas
        if (token->type != TOKEN_COMMA) {
            return false;
        }
    }

    return right_side_function(&left_side_ids);
}

// TODO: Make this function flexible enough to handle returns, calls and assignments properly
static bool right_side_function(sll_s *left_side_ids)
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_ID) {
        T_token *token2 = get_next_token();
        if (token2->type == TOKEN_LEFT_BRACKET) {
            unget_token(token2);
            unget_token(token);

            /* TODO: code-gen gen_push_arg */
            return rule_CALL();
            /* TODO: code-gen gen_pop_ret */
        }
        unget_token(token2);
    }

    unget_token(token);

    while (true) {

        if (!rule_EXPR()) {
            return false;
        }

        /* TODO: semantic check types */
        /* TODO: code-gen gen_var_assign */

        sll_delete_head(left_side_ids, false);

        token = get_next_token();

        if (token->type != TOKEN_COMMA) {
            unget_token(token);
            break;
        }
    }

    return sll_is_empty(left_side_ids);
}
