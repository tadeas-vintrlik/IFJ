/**
 * @file tests.h
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for common test functions.
 */

#ifndef _TESTS_H_
#define _TESTS_H_

#include "token_stack.h"

static T_token *create_token(const char *key, token_type type)
{
    T_token *new;
    dynamic_string_s *ds;

    new = malloc(sizeof *new);
    assert_non_null(new);
    ds = malloc(sizeof *ds);
    assert_non_null(ds);

    ds_init(ds);
    for (unsigned i = 0; key[i]; i++) {
        ds_add_char(ds, key[i]);
    }
    ds_add_char(ds, '\0');
    new->value = ds;
    new->type = type;

    return new;
}

#endif /* _TESTS_H_ */
