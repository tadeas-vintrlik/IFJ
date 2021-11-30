/**
 * @file tests.h
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for common test functions.
 */

#ifndef _TESTS_H_
#define _TESTS_H_

#include "token_stack.h"

static T_token *create_token(const char *key, token_type type)
{
    T_token *new;

    new = malloc(sizeof *new);
    token_init(new);

    for (unsigned i = 0; key[i]; i++) {
        ds_add_char(new->value, key[i]);
    }
    new->type = type;
    new->fun_info = malloc(sizeof *new->fun_info);
    function_info_init(new->fun_info);
    new->fun_info->defined = true;

    return new;
}

#endif /* _TESTS_H_ */
