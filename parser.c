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
#include "semantics.h"
#include "sll.h"
#include "symtable.h"
#include "token_stack.h"
#include <stdbool.h>

static rc_e rc = RC_SYN_ERR;

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
static bool rule_CALL(bool top_level);

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
static bool rule_EXPR(symbol_type_e *type);
static bool rule_VAR_DECL();

static bool left_side_function();
static bool right_side_function(sll_s *left_side_ids);
static bool assign_call_to_left_ids(sll_s *left_side_ids, T_token *fun_symbol, unsigned line);
static bool assign_expressions_to_left_ids(sll_s *left_side_ids, unsigned line);
static bool evaluate_return_expressions(unsigned line);

rc_e start_parsing()
{
    rc_e ret;
    symtable_init(&symtable);
    ret = rule_PROG() ? RC_OK : rc;
    gen_function_call_list();
    symtable_destroy(&symtable);
    return ret;
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
        return rule_CALL(true);
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

static bool rule_CALL(bool top_level)
{
    T_token *token, *function;
    tstack_s *in_params = malloc(sizeof *in_params);
    ALLOC_CHECK(in_params);
    tstack_init(in_params);

    GET_CHECK(TOKEN_ID);
    unsigned line = token->line;
    if (!sem_check_call_function(token, &symtable, &function, &rc)) {
        return false;
    }
    token_destroy(token);

    GET_CHECK(TOKEN_LEFT_BRACKET);
    token_destroy(token);

    if (!rule_ARG_LIST(in_params)) {
        return false;
    }

    // NOTE: The write(...) function can never have a semantic error
    if (strcmp("write", function->value->content)
        && !sem_call_types_compatible(function, in_params, line, &rc)) {
        return false;
    }

    if (top_level) {
        gen_call_insert(function, in_params);
    } else {
        gen_func_call(function->value->content, in_params);
    }

    GET_CHECK(TOKEN_RIGHT_BRACKET);
    token_destroy(token);

    return true;
}

static bool rule_DECL()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "global");
    token_destroy(token);

    GET_CHECK(TOKEN_ID);

    if (!sem_check_redecl(token, &symtable, &rc)) {
        return false;
    }

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
    T_token *token, *function_symbol = NULL;

    GET_CHECK_CMP(TOKEN_KEYWORD, "function");
    token_destroy(token);

    GET_CHECK(TOKEN_ID);
    if (!sem_check_redef(token, &symtable, &function_symbol, &rc)) {
        if (function_symbol->fun_info->defined) {
            return false;
        }
    } else {
        /* First time definition */
        token->fun_info = malloc(sizeof(function_info_s));
        ALLOC_CHECK(token->fun_info);
        function_info_init(token->fun_info);
        token->fun_info->defined = true;

        symtable_insert_token_global(&symtable, token);
        function_symbol = token;
    }

    GET_CHECK(TOKEN_LEFT_BRACKET);
    token_destroy(token);
    symtable_new_frame(&symtable);

    tstack_s *defined_params_in = malloc(sizeof(tstack_s));
    ALLOC_CHECK(defined_params_in);
    tstack_init(defined_params_in);
    if (!rule_PARAM_LIST(defined_params_in)) {
        return false;
    }

    GET_CHECK(TOKEN_RIGHT_BRACKET);
    token_destroy(token);

    tstack_s *defined_types_out = malloc(sizeof(tstack_s));
    ALLOC_CHECK(defined_types_out);
    tstack_init(defined_types_out);
    if (!rule_RET_LIST(defined_types_out)) {
        return false;
    }

    /* If defining a pre-declared function check the types */
    if (!function_symbol->fun_info->defined) {
        if (!sem_check_decl_def_params(function_symbol, defined_params_in, &rc)) {
            return false;
        }

        if (!sem_check_decl_def_returns(function_symbol, defined_params_in, &rc)) {
            return false;
        }
        function_symbol->fun_info->defined = true;
    }

    // Must copy in params as they are also on the local frame (so they don't get destroyed)
    tstack_s *copy = tstack_copy(defined_params_in);
    FREE(defined_params_in);
    function_symbol->fun_info->in_params = copy;
    function_symbol->fun_info->out_params = defined_types_out;

    gen_func_start(function_symbol->value->content, copy, sll_get_length(defined_types_out));
    if (!rule_BODY()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    symtable_pop_frame(&symtable);
    gen_func_end(defined_types_out);
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

static bool rule_ARG_LIST(tstack_s *in_params)
{
    if (!rule_ARG(in_params)) {
        return true;
    }

    return rule_NEXT_ARG(in_params);
}

static bool rule_ARG(tstack_s *in_params)
{
    T_token *token = get_next_token();
    T_token *symbol;

    switch (token->type) {
    case TOKEN_ID:
        if (!sem_check_id_decl(token, &symtable, &symbol, &rc)) {
            return false;
        }
        token->symbol_type = symbol->symbol_type;
        break;
    case TOKEN_NUMBER:
        token->symbol_type = SYM_TYPE_NUMBER;
        break;
    case TOKEN_INT:
        token->symbol_type = SYM_TYPE_INT;
        break;
    case TOKEN_STRING:
        token->symbol_type = SYM_TYPE_STRING;
        break;
    case TOKEN_KEYWORD:
        if (!strcmp("nil", token->value->content)) {
            token->symbol_type = SYM_TYPE_NIL;
            break;
        }

        unget_token(token);
        return false;
    default:
        unget_token(token);
        return false;
    }

    sll_insert_last(in_params, token);
    return true;
}

static bool rule_NEXT_ARG(tstack_s *in_params)
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
            // TODO: Refactor
            switch (token->type) {
            case TOKEN_ID:
            case TOKEN_NUMBER:
            case TOKEN_INT:
            case TOKEN_STRING:
                unget_token(token);
                if (!evaluate_return_expressions(token->line)) {
                    return false;
                }
                printf("POPFRAME\nRETURN\n");
                return true && rule_STATEMENT_LIST();
            case TOKEN_KEYWORD:
                if (!strcmp("nil", token->value->content)) {
                    unget_token(token);
                    if (!evaluate_return_expressions(token->line)) {
                        return false;
                    }
                    printf("POPFRAME\nRETURN\n");
                    return true && rule_STATEMENT_LIST();
                }

                unget_token(token);
                printf("POPFRAME\nRETURN\n");
                return true && rule_STATEMENT_LIST();
            default:
                unget_token(token);
                printf("POPFRAME\nRETURN\n");
                return true && rule_STATEMENT_LIST();
            }
            // TODO: Refactor
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

    if (!rule_EXPR(NULL)) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "then");
    unsigned label = gen_jump_else();
    token_destroy(token);

    symtable_new_frame(&symtable);
    if (!rule_BODY()) {
        return false;
    }
    symtable_pop_frame(&symtable);

    GET_CHECK_CMP(TOKEN_KEYWORD, "else");
    gen_else_label(label);
    token_destroy(token);

    symtable_new_frame(&symtable);
    if (!rule_BODY()) {
        return false;
    }
    symtable_pop_frame(&symtable);

    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    gen_if_end(label);
    token_destroy(token);

    return true;
}

static bool rule_WHILE()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "while");
    unsigned label = gen_while_label();
    token_destroy(token);

    if (!rule_EXPR(NULL)) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "do");
    gen_while_jump_end(label);
    token_destroy(token);

    symtable_new_frame(&symtable);
    if (!rule_BODY()) {
        return false;
    }
    symtable_pop_frame(&symtable);

    gen_while_jump_loop(label);
    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    gen_while_end_label(label);
    token_destroy(token);

    return true;
}

static bool rule_VAR_DECL()
{
    T_token *token;
    // left_side_ids always contains exactly one ID
    sll_s left_side_ids;
    sll_init(&left_side_ids);

    GET_CHECK_CMP(TOKEN_KEYWORD, "local");
    token_destroy(token);

    GET_CHECK(TOKEN_ID);
    T_token *symbol = token;
    if (!sem_check_id_redecl(token, &symtable, &rc)) {
        return false;
    }

    gen_var_decl(token);

    sll_insert_head(&left_side_ids, token);

    GET_CHECK(TOKEN_COLON);
    token_destroy(token);

    tstack_s collected_type;
    tstack_init(&collected_type);

    if (!rule_TYPE(&collected_type)) {
        return false;
    }

    symbol->symbol_type = tstack_top(&collected_type)->symbol_type;
    symtable_insert_token_top(&symtable, token_copy(symbol));
    tstack_destroy(&collected_type);

    token = get_next_token();

    if (token->type == TOKEN_DECLAR) {
        token_destroy(token);
        // left_side_ids always contains exactly one ID
        return right_side_function(&left_side_ids);
    } else {
        // TODO:Codegen assign nil to this new variable
        unget_token(token);
        return true;
    }
}

static bool rule_EXPR(symbol_type_e *type) { return exp_parse(&symtable, &rc, type); }

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

        sll_insert_last(&left_side_ids, token);

        // TODO: Move to semantics?
        T_token *symbol;
        if (!symtable_search_all(&symtable, token->value->content, &symbol)) {
            ERR_MSG("Assigning to undeclared variable.", token->line);
            rc = RC_SEM_UNDEF_ERR;
            return false;
        }
        token->symbol_type = symbol->symbol_type;

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

/**
 * @brief Evaluates function call or expressions on right side and assigns them to ids.
 *
 * @param[in] left_side_ids List of IDs to assign to. If empty, results of eval are discarded.
 *
 * @return true Right side is syntactically correct.
 * @return false Right side is syntactically incorrect.
 */
static bool right_side_function(sll_s *left_side_ids)
{
    T_token *token = get_next_token();
    T_token *token_2 = get_next_token();
    unget_token(token_2);
    unget_token(token);

    /* Has to be allocated separately since rule_CALL frees the tokens */
    char *func_name = my_strdup(token->value->content);
    ALLOC_CHECK(func_name);

    unsigned line = token->line;

    bool left_side_empty = sll_is_empty(left_side_ids);
    bool is_call = token->type == TOKEN_ID && token_2->type == TOKEN_LEFT_BRACKET;

    if (left_side_empty && is_call) {
        // handles "fun(a, b)"
        FREE(func_name);
        return rule_CALL(false);
    } else if (!left_side_empty && is_call) {
        // handles "a, b, c = fun(a, b)"

        if (!rule_CALL(false)) {
            return false;
        }

        T_token *fun_symbol;
        // Existence checked in rule_CALL
        symtable_search_global(&symtable, func_name, &fun_symbol);
        FREE(func_name);

        return assign_call_to_left_ids(left_side_ids, fun_symbol, line);
    } else if (!left_side_empty && !is_call) {
        // handles "a, b = b, a..."
        FREE(func_name);
        return assign_expressions_to_left_ids(left_side_ids, line);
    }

    assert(false);
    FREE(func_name);
    return sll_is_empty(left_side_ids);
}

static bool assign_call_to_left_ids(sll_s *left_side_ids, T_token *fun_symbol, unsigned line)
{
    if (!sem_check_call_assign(left_side_ids, fun_symbol->fun_info->out_params, line, &rc)) {
        return false;
    }

    sll_activate(left_side_ids);
    sll_activate(fun_symbol->fun_info->out_params);

    unsigned ret_index = 0;

    while (!tstack_empty(left_side_ids)) {
        T_token *id = tstack_top(left_side_ids);

        printf("MOVE LF@%s TF@%%retval%d\n", id->value->content, ret_index);
        ret_index++;

        tstack_pop(left_side_ids, true);
        sll_next(fun_symbol->fun_info->out_params);
    }

    return true;
}

static bool assign_expressions_to_left_ids(sll_s *left_side_ids, unsigned line)
{
    sll_activate(left_side_ids);

    while (sll_is_active(left_side_ids)) {
        T_token *token = get_next_token();

        /* Skip commas between expressions */
        if (token->type == TOKEN_COMMA) {
            token_destroy(token);
        } else {
            unget_token(token);
        }

        symbol_type_e expr_type = SYM_TYPE_NIL;
        if (!rule_EXPR(&expr_type)) {
            return false;
        }

        T_token *left = sll_get_active(left_side_ids);
        if (!sem_check_type_compatible(left->symbol_type, expr_type)) {
            ERR_MSG("Assigning to invalid type.\n", line);
            rc = RC_SEM_ASSIGN_ERR;
            return false;
        }

        sll_next(left_side_ids);
    }

    tstack_reverse(&left_side_ids);
    sll_activate(left_side_ids);

    while (!tstack_empty(left_side_ids)) {
        T_token *id = tstack_top(left_side_ids);

        printf("POPS LF@%s\n", id->value->content);
        tstack_pop(left_side_ids, true);
    }

    return true;
}

static bool evaluate_return_expressions(unsigned line)
{
    T_token *defined_func = symtable_get_current_def(&symtable);
    sll_s *out_params = defined_func->fun_info->out_params;
    sll_activate(out_params);

    unsigned expr_index = 0;
    while (sll_is_active(out_params)) {
        T_token *token = get_next_token();

        /* Skip commas between expressions */
        if (token->type == TOKEN_COMMA) {
            token_destroy(token);
        } else {
            unget_token(token);
        }

        symbol_type_e expr_type;
        if (!rule_EXPR(&expr_type)) {
            if (rc == RC_OK) {
                /* TODO: Is this the right error? */
                ERR_MSG("Not enough return values for function.\n", line);
                rc = RC_SEM_OTHER_ERR;
            }
            return false;
        }

        T_token *out_param = sll_get_active(out_params);
        if (out_param->symbol_type != expr_type) {
            /* TODO: Is this the right error? */
            /* TODO: Should it not be compatible as well? */
            ERR_MSG("Return type incompatible with function definition.\n", line);
            rc = RC_SEM_OTHER_ERR;
            return false;
        }

        printf("POPS LF@%%retval%d\n", expr_index);

        expr_index++;
        sll_next(out_params);
    }

    return true;
}
