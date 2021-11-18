/**
 * @file token_stack.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of stack of tokens.
 */

#include "token_stack.h"

void token_destroy(T_token *token)
{
    if (!token) {
        return;
    }
    ds_destroy(token->value);
    FREE(token->value);
    FREE(token);
}

void tstack_init(tstack_s *tstack) { sll_init((sll_s *)tstack); }

void tstack_push(tstack_s *tstack, T_token *token) { sll_insert_head((sll_s *)tstack, token); }

T_token *tstack_top(tstack_s *tstack) { return sll_get_head((sll_s *)tstack); }

void tstack_pop(tstack_s *tstack)
{
    token_destroy(sll_get_head(tstack));
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