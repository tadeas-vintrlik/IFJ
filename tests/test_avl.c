/**
 * @file test_avl.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Test for the functionality of AVL Trees.
 */
#define _GNU_SOURCE
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "avl.h"

#define ALLOC_KEY(in, out)\
    out = my_strdup(in);\
    assert_non_null(out);
#define GET_VALUE(x) (*(int *)x->value)
#define CAST_INT(x) (*(int *)x)

typedef struct test_state {
    avl_node_s *node;
} ts_s;

/**
 * @brief Get the balance of the AVL Tree node.
 *
 * @param[in] node The node to get balance of.
 *
 * @return Balance of the node.
 */
static int avl_node_get_balance(avl_node_s *node)
{
    if (!node) {
        return 0;
    }
    if (node->left && node->right) {
        return node->right->height - node->left->height;
    }
    if (node->left) {
        return -node->left->height;
    }
    if (node->right) {
        return node->right->height;
    }
    return 0;
}

static int global_setup(void **state)
{
    ts_s *ts;
    ts = malloc(sizeof *ts);
    if (!ts) {
        return 1;
    }
    *state = ts;
    return 0;
}

static int global_teardown(void **state)
{
    ts_s *ts = *state;
    free(ts);
    return 0;
}

static void test_init(void **state)
{
    ts_s *ts = *state;
    avl_init(&ts->node);
    assert_null(ts->node);
}

static void test_insert(void **state)
{
    ts_s *ts = *state;
    char *key;
    int *data = malloc(sizeof *data);

    /* Check test preconditions */
    assert_non_null(data);
    assert_null(ts->node);

    /* Insert a node */
    ALLOC_KEY("5", key);
    *data = 5;
    avl_insert(&ts->node, key, data);

    /* Check if inserted correctly */
    assert_non_null(ts->node);
    assert_string_equal(ts->node->key, "5");
    assert_int_equal(ts->node->value, data);
    assert_int_equal(GET_VALUE(ts->node), 5);
    assert_null(ts->node->left);
    assert_null(ts->node->right);
    assert_int_equal(avl_node_get_balance(ts->node), 0);
}

static void test_insert_order(void **state)
{
    char *key4, *key6;
    ts_s *ts = *state;

    /* Should get inserted on the left - balance checked accordingly */
    ALLOC_KEY("4", key4);
    avl_insert(&ts->node, key4, NULL);
    assert_non_null(ts->node->left);
    assert_null(ts->node->right);
    assert_int_equal(avl_node_get_balance(ts->node), -1);
    assert_string_equal(ts->node->left->key, "4");

    /* Should get inserted on the right - should be balanced again */
    ALLOC_KEY("6", key6);
    avl_insert(&ts->node, key6, NULL);
    assert_non_null(ts->node->left);
    assert_non_null(ts->node->right);
    assert_int_equal(avl_node_get_balance(ts->node), 0);
    assert_string_equal(ts->node->right->key, "6");
}

static void test_search(void **state)
{
    ts_s *ts = *state;
    void *value;

    /*
     *    |- 6
     * 5 -
     *    |- 4
     */

    /* Find root and check if returned correct value */
    assert_true(avl_search(ts->node, "5", &value));
    assert_int_equal(CAST_INT(value), 5);

    /* Try passing NULL as out param - should just return true or false */
    assert_true(avl_search(ts->node, "5", NULL));

    /* Try finding a non-existent key */
    assert_false(avl_search(ts->node, "12", NULL));
    assert_false(avl_search(ts->node, "3", &value));

    /* Try finding both children */
    assert_true(avl_search(ts->node, "4", NULL));
    assert_true(avl_search(ts->node, "6", NULL));
}

static void test_delete_nonexistant(void **state)
{
    ts_s *ts = *state;

    /*
     *    |- 6
     * 5 -
     *    |- 4
     */

    /* Deleting a non-existant key should change nothing and return false */
    assert_false(avl_delete(&ts->node, "12", NULL));
    assert_non_null(ts->node);
    assert_non_null(ts->node->left);
    assert_non_null(ts->node->right);
}

static void test_delete_terminal(void **state)
{
    ts_s *ts = *state;
    char *key4;

    /*
     *    |- 6
     * 5 -
     *    |- 4
     */

    /* Delete left child - check if left and balanced have changed accordingly */
    assert_true(avl_delete(&ts->node, "4", NULL));
    assert_null(ts->node->left);
    assert_int_equal(avl_node_get_balance(ts->node), 1);

    /*
     *    |- 6
     * 5 -
     *
     */

    /* Reinsert it back */
    ALLOC_KEY("4", key4)
    avl_insert(&ts->node, key4, NULL);
}

static void test_delete_one_child(void **state)
{
    ts_s *ts = *state;
    char *key3;

    /*
     *    |- 6
     * 5 -
     *    |- 4
     */

    /* Insert another node */
    ALLOC_KEY("3", key3);
    avl_insert(&ts->node, key3, NULL);
    assert_string_equal(ts->node->left->left->key, "3");
    assert_int_equal(avl_node_get_balance(ts->node), -1);

    /*
     *    |- 6
     * 5 -|
     *    |- 4 -|
     *          |-3
     */

    /* Delete node with key 4 to check if node with key 3 is relinked correctly */
    assert_true(avl_delete(&ts->node, "4", NULL));
    assert_string_equal(ts->node->left->key, "3");
    assert_int_equal(avl_node_get_balance(ts->node), 0);

    /*
     *    |- 6
     * 5 -|
     *    |- 3
     */
}

static void test_delete_two_children(void **state)
{
    ts_s *ts = *state;

    /*
     *    |- 6
     * 5 -|
     *    |- 3
     */

    /* Try deleting the root */
    assert_true(avl_delete(&ts->node, "5", NULL));

    /*
     *    |- 6
     * 3 -|
     */

    /* Check for correct reordering */
    assert_string_equal(ts->node->key, "3");
    assert_string_equal(ts->node->right->key, "6");
    assert_null(ts->node->left);
    assert_int_equal(avl_node_get_balance(ts->node), 1);
}

static void test_insert_rebalance(void **state)
{
    ts_s *ts = *state;
    char *key2, *key1, *key0;

    /*
     *    |- 6
     * 3 -|
     */

    assert_int_equal(avl_node_get_balance(ts->node), 1);

    ALLOC_KEY("2", key2);
    avl_insert(&ts->node, key2, NULL);
    assert_int_equal(avl_node_get_balance(ts->node), 0);
    ALLOC_KEY("1", key1);
    avl_insert(&ts->node, key1, NULL);
    assert_int_equal(avl_node_get_balance(ts->node), -1);

    /*
     *    |- 6
     * 3 -|
     *    |- 2 -|
     *          |- 1
     */

    ALLOC_KEY("0", key0);
    avl_insert(&ts->node, key0, NULL);

    /*
     *    |- 6
     * 3 -|
     *    |- 2 -|
     *          |- 1 -|
     *                |- 0
     */

    /* This should cause a rebalancing...
     *    |- 6
     * 3 -|     |- 2
     *    |- 1 -|
     *          |- 0
     */
    assert_string_equal(ts->node->key, "3");
    assert_int_equal(avl_node_get_balance(ts->node), -1);
    assert_int_equal(avl_node_get_balance(ts->node->left), 0);
    assert_int_equal(avl_node_get_balance(ts->node->right), 0);
    assert_string_equal(ts->node->right->key, "6");
    assert_string_equal(ts->node->left->key, "1");
    assert_string_equal(ts->node->left->right->key, "2");
    assert_string_equal(ts->node->left->left->key, "0");
}

static void test_delete_rebalace(void **state)
{
    ts_s *ts = *state;

    /*
     *    |- 6
     * 3 -|     |- 2
     *    |- 1 -|
     *          |- 0
     */

    assert_true(avl_delete(&ts->node, "6", NULL));

    /* This should cause rebalancing...
     * 3 -|     |- 2
     *    |- 1 -|
     *          |- 0
     */

    /*
     *    |- 3 -|
     * 1 -|     |- 2
     *    |- 0
     */

    assert_string_equal(ts->node->key, "1");
    assert_int_equal(avl_node_get_balance(ts->node), 1);
    assert_string_equal(ts->node->left->key, "0");
    assert_string_equal(ts->node->right->key, "3");
    assert_string_equal(ts->node->right->left->key, "2");
}

static void test_rl_rotate(void **state)
{
    ts_s *ts = *state;

    /*
     *    |- 3 -|
     * 1 -|     |- 2
     *    |- 0
     */

    assert_true(avl_delete(&ts->node, "0", NULL));

    /* This should cause rebalancing...
     *    |- 3 -|
     * 1 -|     |- 2
     */

    /*
     *    |- 3
     * 2 -|
     *    |- 1
     */
    assert_string_equal(ts->node->key, "2");
    assert_int_equal(avl_node_get_balance(ts->node), 0);
    assert_string_equal(ts->node->left->key, "1");
    assert_string_equal(ts->node->right->key, "3");
}

static void test_destroy(void **state)
{
    ts_s *ts = *state;

    assert_non_null(ts->node);
    avl_destroy(&ts->node, NULL);
    assert_null(ts->node);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init),
        cmocka_unit_test(test_insert),
        cmocka_unit_test(test_insert_order),
        cmocka_unit_test(test_search),
        cmocka_unit_test(test_delete_nonexistant),
        cmocka_unit_test(test_delete_terminal),
        cmocka_unit_test(test_delete_one_child),
        cmocka_unit_test(test_delete_two_children),
        cmocka_unit_test(test_insert_rebalance),
        cmocka_unit_test(test_delete_rebalace),
        cmocka_unit_test(test_rl_rotate),
        cmocka_unit_test(test_destroy),
    };
    return cmocka_run_group_tests(tests, global_setup, global_teardown);
}
