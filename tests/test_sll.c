/**
 * @file test_sll.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Test for the functionality of Single Linked List structure.
 */
#define _GNU_SOURCE
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sll.h"

typedef struct test_state {
    sll_s *list;
} ts_s;

static int local_setup(void **state)
{
    ts_s *ts;
    ts = malloc(sizeof *ts);
    if (!ts) {
        return 1;
    }
    ts->list = malloc(sizeof *ts->list);
    if (!ts->list) {
        return 1;
    }
    *state = ts;
    return 0;
}

static int local_teardown(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;
    sll_destroy(list, true);
    free(ts->list);
    free(ts);
    return 0;
}

static void test_init(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    sll_init(list);
    assert_null(list->head);
    assert_null(list->active);
}

static void test_is_empty(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    assert_true(sll_is_empty(list));
}

static void test_insert_head(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;
    char *first, *second;
    first = strdup("World");
    second = strdup("Hello");

    sll_insert_head(list, first);

    /* It should not be empty anymore */
    assert_false(sll_is_empty(list));
    /* Check if storing the right value */
    assert_string_equal(first, (char *)sll_get_head(list));

    sll_insert_head(list, second);

    /* It still should not be empty */
    assert_false(sll_is_empty(list));
    /* Check if inserted as new first */
    assert_string_equal(second, (char *)sll_get_head(list));
}

static void test_delete_head(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    /* There should be two elements from previous test */
    assert_false(sll_is_empty(list));

    /* Delete first should not be empty */
    sll_delete_head(list, true);
    assert_false(sll_is_empty(list));

    /* Delete second should be empty */
    sll_delete_head(list, true);
    assert_true(sll_is_empty(list));
}

static void test_activate_empty(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    /* By default the list should be inactive */
    assert_false(sll_is_active(list));

    /* Activate on an empty list should do noting */
    assert_true(sll_is_empty(list));
    sll_activate(list);
    assert_false(sll_is_active(list));
}

static int setup_test_activate(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;
    char *first;

    first = strdup("World");
    sll_insert_head(list, first);

    return 0;
}

static void test_activate(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    /* By default the list should be inactive */
    assert_false(sll_is_active(list));

    /* Now first should be active */
    sll_activate(list);
    assert_true(sll_is_active(list));
    assert_int_equal(sll_get_active(list), sll_get_head(list));
}

static int teardown_test_activate(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    sll_delete_head(list, true);
    assert_true(sll_is_empty(list));

    return 0;
}

static void test_lose_activity(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    /* Beginning of the test is same as previous */
    test_activate(state);

    /* Removing active should lead to losing activity */
    sll_delete_head(list, true);
    assert_false(sll_is_active(list));
    assert_true(sll_is_empty(list));
}

static int setup_list_three(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;
    char *first, *second, *third;

    assert_true(sll_is_empty(list));

    first = strdup("World");
    second = strdup("List");
    third = strdup("Hello");

    sll_insert_head(list, first);
    sll_insert_head(list, second);
    sll_insert_head(list, third);

    return 0;
}

static void test_next(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    /* List should have no activity */
    assert_false(sll_is_active(list));

    /* Activate the list check first element */
    sll_activate(list);
    assert_true(sll_is_active(list));
    assert_string_equal("Hello", sll_get_active(list));

    /* Move activity to the next element */
    sll_next(list);
    assert_true(sll_is_active(list));
    assert_string_equal("List", sll_get_active(list));

    /* Move activity to the last element */
    sll_next(list);
    assert_true(sll_is_active(list));
    assert_string_equal("World", sll_get_active(list));

    /* After moving from the last element activity should be lost */
    sll_next(list);
    assert_false(sll_is_active(list));
}

static int teardown_list_three(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    sll_delete_head(list, true);
    sll_delete_head(list, true);
    sll_delete_head(list, true);

    assert_true(sll_is_empty(list));

    return 0;
}

static void test_after(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;
    char *data;

    /* List should have no activity */
    assert_false(sll_is_active(list));

    /* Move activity to the last element */
    sll_activate(list);
    sll_next(list);
    sll_next(list);

    /* Getting element after last should return NULL */
    assert_null(sll_get_after(list));

    /* Insert element after active check for value */
    data = strdup("!");
    sll_insert_after(list, data);
    assert_string_equal("!", (char *)sll_get_after(list));

    /* Delete element after active, active is last yet again */
    sll_delete_after(list, true);
    assert_null(sll_get_after(list));

    /* Delete after active when there is no such element should do nothing */
    sll_delete_after(list, true);
    assert_null(sll_get_after(list));

    /* Lose activity, check if functions handle it okay */
    assert_true(sll_is_active(list));
    sll_next(list);
    assert_false(sll_is_active(list));

    assert_null(sll_get_after(list));
    sll_insert_after(list, data);
    sll_delete_after(list, true);
}

static void test_length(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;

    assert_int_equal(sll_get_length(list), 3);

    sll_delete_head(list, true);
    assert_int_equal(sll_get_length(list), 2);

    sll_delete_head(list, true);
    assert_int_equal(sll_get_length(list), 1);

    sll_delete_head(list, true);
    assert_int_equal(sll_get_length(list), 0);
}

static void test_last(void **state)
{
    ts_s *ts = *state;
    sll_s *list = ts->list;
    char *insert;

    assert_string_equal("World", sll_get_last(list));

    insert = strdup("!");
    sll_insert_last(list, insert);
    assert_string_equal("!", sll_get_last(list));

    sll_delete_last(list, true);
    assert_string_equal("World", sll_get_last(list));

    sll_delete_last(list, true);
    assert_string_equal("List", sll_get_last(list));

    sll_delete_last(list, true);
    assert_string_equal("Hello", sll_get_last(list));

    sll_delete_last(list, true);
    assert_null(sll_get_last(list));

    insert = strdup("!");
    sll_insert_last(list, insert);
    assert_string_equal("!", sll_get_last(list));
    assert_string_equal("!", sll_get_head(list));

    sll_delete_last(list, true);
    assert_int_equal(0, sll_get_length(list));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init),
        cmocka_unit_test(test_is_empty),
        cmocka_unit_test(test_insert_head),
        cmocka_unit_test(test_delete_head),
        cmocka_unit_test(test_activate_empty),
        cmocka_unit_test_setup_teardown(test_activate, setup_test_activate, teardown_test_activate),
        cmocka_unit_test_setup(test_lose_activity, setup_test_activate),
        cmocka_unit_test_setup_teardown(test_next, setup_list_three, teardown_list_three),
        cmocka_unit_test_setup_teardown(test_after, setup_list_three, teardown_list_three),
        cmocka_unit_test_setup(test_length, setup_list_three),
        cmocka_unit_test_setup(test_last, setup_list_three),
    };
    return cmocka_run_group_tests(tests, local_setup, local_teardown);
}
