/**
 * @file test_symtable.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Test for the functionality of Table of symbols.
 */
#define _GNU_SOURCE
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "symtable.h"

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

typedef struct state {
    symtable_s *symtable;
} st_s;

static int local_setup(void **state)
{
    st_s *st;

    st = malloc(sizeof *st);
    if (!st) {
        return 1;
    }
    st->symtable = malloc(sizeof *st->symtable);
    if (!st->symtable) {
        return 1;
    }
    symtable_init(st->symtable);
    if (!st->symtable) {
        return 1;
    }

    *state = st;
    return 0;
}

static int local_teardown(void **state)
{
    st_s *st = *state;

    symtable_destroy(st->symtable);
    if (st->symtable->global) {
        return 1;
    }
    free(st->symtable);
    free(st);

    return 0;
}

static void test_search_empty(void **state)
{
    st_s *st = *state;

    assert_false(symtable_search_all(st->symtable, "random-key", NULL));
    assert_false(symtable_search_top(st->symtable, "random-key", NULL));
    assert_false(symtable_search_global(st->symtable, "random-key", NULL));
}

static void test_insert(void **state)
{
    st_s *st = *state;
    T_token *token, *out;

    /* Create and insert a new token into global frame */
    token = create_token("func1", TOKEN_ID);
    symtable_insert_token_global(st->symtable, token);

    /* Should find it in global frame */
    assert_true(symtable_search_global(st->symtable, "func1", &out));
    assert_int_equal(out->type, TOKEN_ID);
    assert_string_equal(out->value->content, "func1");

    /* Should find it in all frames as well */
    assert_true(symtable_search_all(st->symtable, "func1", &out));
    assert_int_equal(out->type, TOKEN_ID);
    assert_string_equal(out->value->content, "func1");

    token = create_token("func2", TOKEN_ID);
    symtable_insert_token_global(st->symtable, token);

    token = create_token("main", TOKEN_ID);
    symtable_insert_token_global(st->symtable, token);

    assert_true(symtable_search_all(st->symtable, "func1", NULL));
    assert_true(symtable_search_all(st->symtable, "func2", NULL));
    assert_true(symtable_search_all(st->symtable, "main", NULL));
}

static void test_new_frame(void **state)
{
    st_s *st = *state;
    T_token *token;

    /**
     * Frames: {}
     * Global: {func1, func2, main}
     */

    /* There should be no local frames yet */
    assert_true(symtable_frames_empty(st->symtable));

    /* Insert a local frame */
    symtable_new_frame(st->symtable);
    assert_false(symtable_frames_empty(st->symtable));

    /* Searching tokens that are in global frame should not succeed on top */
    assert_false(symtable_search_top(st->symtable, "main", NULL));

    /* But it should still work on all frames */
    assert_true(symtable_search_all(st->symtable, "main", NULL));

    /* Create and insert a new token into top frame */
    token = create_token("x", TOKEN_ID);
    symtable_insert_token_top(st->symtable, token);

    /* Should find it in top and all frames */
    assert_true(symtable_search_top(st->symtable, "x", NULL));
    assert_true(symtable_search_all(st->symtable, "x", NULL));

    /* But it should not be in global */
    assert_false(symtable_search_global(st->symtable, "x", NULL));

    /* Insert yet another frame x should not be in top frame now */
    symtable_new_frame(st->symtable);
    assert_false(symtable_search_top(st->symtable, "x", NULL));
}

static void test_pop_frame(void **state)
{
    st_s *st = *state;

    /**
     * Frames: {{}, {x}}
     * Global: {func1, func2, main}
     */

    /* There should be a frame */
    assert_false(symtable_frames_empty(st->symtable));
    symtable_pop_frame(st->symtable);

    /* There still should be a frame but x should be in top frame again */
    assert_false(symtable_frames_empty(st->symtable));
    assert_true(symtable_search_top(st->symtable, "x", NULL));

    /* Pop once again and there should be none */
    symtable_pop_frame(st->symtable);
    assert_true(symtable_frames_empty(st->symtable));
    assert_false(symtable_search_top(st->symtable, "x", NULL));

    /* Popping again should do nothing */
    symtable_pop_frame(st->symtable);
    assert_true(symtable_frames_empty(st->symtable));
    assert_false(symtable_search_top(st->symtable, "x", NULL));

    /* Global tokens should still be accessible */
    assert_true(symtable_search_all(st->symtable, "main", NULL));
}

static void test_frame_priority(void **state)
{
    st_s *st = *state;
    T_token *token, *local, *global;

    /**
     * Frames: {}
     * Global: {func1, func2, main}
     */

    /* Insert a local frame */
    symtable_new_frame(st->symtable);
    assert_false(symtable_frames_empty(st->symtable));

    /* Create and insert a new token into a local frame with same name as one in global */
    token = create_token("func1", TOKEN_ID);
    symtable_insert_token_top(st->symtable, token);

    /* The token found in global and top should not be the same */
    assert_true(symtable_search_top(st->symtable, "func1", &local));
    assert_true(symtable_search_global(st->symtable, "func1", &global));
    assert_int_not_equal(local, global);

    /* The token found in all should be the topmost */
    assert_true(symtable_search_all(st->symtable, "func1", &token));
    assert_int_equal(token, local);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_search_empty),
        cmocka_unit_test(test_insert),
        cmocka_unit_test(test_new_frame),
        cmocka_unit_test(test_pop_frame),
        cmocka_unit_test(test_frame_priority),
    };
    return cmocka_run_group_tests(tests, local_setup, local_teardown);
}