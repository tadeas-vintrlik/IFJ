/**
 * @file token_stack.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of stack of tokens.
 */

#include "token_stack.h"

void token_init(T_token *token)
{
    token->value = malloc(sizeof(dynamic_string_s));
    ALLOC_CHECK(token->value);

    ds_init(token->value);

    token->type = TOKEN_ID;
    token->line = 0;
    token->symbol_type = SYM_TYPE_NONE;
    token->fun_info = NULL;
}

T_token *token_copy(T_token *original)
{
    T_token *result = malloc(sizeof(T_token));
    ALLOC_CHECK(result);

    token_init(result);

    result->line = original->line;
    result->symbol_type = original->symbol_type;
    result->type = original->type;

    for (unsigned i = 0; i < original->value->size; i++) {
        ds_add_char(result->value, original->value->content[i]);
    }

    // NOTE: Assuming this won't ever be used for function symbols/tokens
    assert(original->fun_info == NULL);

    return result;
}

void function_info_init(function_info_s *fun_info)
{
    fun_info->defined = false;

    fun_info->in_params = malloc(sizeof(sll_s));
    ALLOC_CHECK(fun_info->in_params);
    sll_init(fun_info->in_params);

    fun_info->out_params = malloc(sizeof(sll_s));
    ALLOC_CHECK(fun_info->out_params);
    sll_init(fun_info->out_params);
}

static void function_info_destroy(function_info_s *fun_info)
{
    if (!fun_info) {
        return;
    }

    tstack_destroy(fun_info->in_params);
    tstack_destroy(fun_info->out_params);
}

void token_destroy(T_token *token)
{
    if (!token) {
        return;
    }
    ds_destroy(token->value);
    function_info_destroy(token->fun_info);

    FREE(token->value);
    FREE(token->fun_info);
    FREE(token);
}

void tstack_init(tstack_s *tstack) { sll_init((sll_s *)tstack); }

tstack_s *tstack_copy(tstack_s *original)
{
    tstack_s *result = malloc(sizeof(tstack_s));
    ALLOC_CHECK(result);
    tstack_init(result);

    sll_activate(original);

    while (sll_is_active(original)) {
        sll_insert_last(result, token_copy(sll_get_active(original)));
        sll_next(original);
    }

    return result;
}

void tstack_push(tstack_s *tstack, T_token *token) { sll_insert_head((sll_s *)tstack, token); }

T_token *tstack_top(tstack_s *tstack) { return sll_get_head((sll_s *)tstack); }

void tstack_pop(tstack_s *tstack, bool destroy)
{
    if (destroy) {
        token_destroy(sll_get_head(tstack));
    }
    sll_delete_head((sll_s *)tstack, false);
}

T_token *tstack_terminal_top(tstack_s *tstack)
{
    sll_s *sll = tstack;
    T_token *token;

    sll_activate(sll);
    do {
        token = sll_get_active(sll);
        if (!token) {
            return NULL;
        }
        if (token->type != TOKEN_HANDLE && token->type != TOKEN_NON_TERMINAL) {
            return token;
        }
        sll_next(sll);
    } while (sll_is_active(sll));

    return NULL;
}

bool tstack_empty(const tstack_s *tstack) { return sll_is_empty(tstack); }

void tstack_reverse(tstack_s **tstack)
{
    tstack_s *new = malloc(sizeof(tstack_s));
    ALLOC_CHECK(new);
    tstack_init(new);

    while (!tstack_empty(*tstack)) {
        tstack_push(new, tstack_top(*tstack));
        tstack_pop(*tstack, false);
    }

    // TODO: Fix this bizzarre double free
    // free(*tstack);
    *tstack = new;
}

void tstack_destroy(tstack_s *tstack)
{
    while (!tstack_empty(tstack)) {
        tstack_pop(tstack, true);
    }
}

static T_token *create_handle(void)
{
    T_token *handle = malloc(sizeof *handle);
    ALLOC_CHECK(handle);

    token_init(handle);

    handle->type = TOKEN_HANDLE;
    handle->line = -1;
    return handle;
}

void tstack_terminal_push(tstack_s *tstack, T_token *token)
{
    T_token *tmp, *handle;

    if (!tstack || !token) {
        return;
    }

    handle = create_handle();
    tmp = tstack_top(tstack);
    tstack_pop(tstack, false);
    if (tmp) {
        if (tmp->type == TOKEN_NON_TERMINAL) {
            tstack_push(tstack, handle);
            tstack_push(tstack, tmp);
            tstack_push(tstack, token);
        } else {
            tstack_push(tstack, tmp);
            tstack_push(tstack, handle);
            tstack_push(tstack, token);
        }
    } else {
        tstack_push(tstack, handle);
        tstack_push(tstack, token);
    }
}
