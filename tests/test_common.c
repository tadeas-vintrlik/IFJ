/**
 * @file test_common.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Test for the functionality of functions and macros in common.
 */
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <cmocka.h>

#include "common.h"

static int local_setup(void **arg)
{
    (void)arg;
    return 0;
}

static int local_teardown(void **arg)
{
    (void)arg;
    return 0;
}

static void test_better_free(void **arg)
{
    void *array = malloc(1);

    (void)arg;
    assert_non_null(array);
    FREE(array);
    assert_null(array);
}

static void test_source_file(void **arg)
{
    source_file_s source;
    const char *test_line = "So Long, and Thanks for All the Fish.";

    (void)arg;
    sf_init(&source);

    /* Check if initialized correctly */
    assert_int_equal(source.no_lines, 0);
    assert_null(source.line[0]);

    sf_add_line(&source, test_line);

    /* Check if line added correctly */
    assert_int_equal(source.no_lines, 1);
    assert_string_equal(source.line[0], test_line);

    /* Check if destroyed correctly */
    sf_destroy(&source);
    assert_null(source.line);
    assert_int_equal(source.no_lines, 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_better_free),
        cmocka_unit_test(test_source_file),
    };
    return cmocka_run_group_tests(tests, local_setup, local_teardown);
}
