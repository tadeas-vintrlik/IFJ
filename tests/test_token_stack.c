/**
 * @file test_token_stack.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Test for the Token Stack.
 */
#define _GNU_SOURCE
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "tests.h"
#include "token_stack.h"

static void test_simple(void **arg)
{
    tstack_s ts;
    T_token *first, *second, *out;

    (void)arg;

    first = create_token("first", TOKEN_ID);
    second = create_token("second", TOKEN_ID);

    /* There should be nothing after init */
    tstack_init(&ts);
    assert_null(tstack_top(&ts));
    assert_true(tstack_empty(&ts));

    /* Push first token */
    tstack_push(&ts, first);

    out = tstack_top(&ts);
    assert_string_equal(out->value->content, "first");
    assert_false(tstack_empty(&ts));

    /* Push second token */
    tstack_push(&ts, second);
    out = tstack_top(&ts);
    assert_string_equal(out->value->content, "second");

    /* Pop token - should be only one now */
    tstack_pop(&ts);
    out = tstack_top(&ts);
    assert_string_equal(out->value->content, "first");

    /* Pop the last one, should be empty */
    tstack_pop(&ts);
    assert_true(tstack_empty(&ts));
    assert_null(tstack_top(&ts));
}

static void test_terminal_top(void **arg)
{
    tstack_s ts;
    T_token *first, *second, *third, *out;

    (void)arg;

    first = create_token("first", TOKEN_NON_TERMINAL);
    second = create_token("second", TOKEN_ID);
    third = create_token("third", TOKEN_HANDLE);

    tstack_init(&ts);

    /* Should return nothing since there is no terminal*/
    tstack_push(&ts, first);
    out = tstack_terminal_top(&ts);
    assert_null(out);

    /* Now it should return the second */
    tstack_push(&ts, second);
    out = tstack_terminal_top(&ts);
    assert_non_null(out);
    assert_string_equal(out->value->content, "second");

    /* Since we inserted a terminal it should still return the same */
    tstack_push(&ts, third);
    out = tstack_terminal_top(&ts);
    assert_non_null(out);
    assert_string_equal(out->value->content, "second");

    tstack_destroy(&ts);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_simple),
        cmocka_unit_test(test_terminal_top),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
