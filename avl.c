/**
 * @file avl.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of AVL Binary Trees.
 *
 * Most of the source was taken from mine (Tadeas Vintrlik) binary search tree implementation
 * in the second IAL homework. Modifications were made to match the AVL tree specification.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "avl.h"
#include "common.h"

/**
 * @brief Enumeration type to determine side of the imbalance.
 */
typedef enum balance {
    BALANCED,
    LEFT_HEAVY,
    RIGHT_HEAVY,
} balance_e;

/**
 * @brief Get the height attribute of the AVL Tree node.
 *
 * @param[in] node The node to get height of.
 *
 * @return Height of the node. 0 if node is NULL.
 */
static int avl_node_get_height(avl_node_s *node)
{
    if (!node) {
        return 0;
    } else {
        return node->height;
    }
}

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

/**
 * @brief Check if the node is leaning to one side (includes a still balance leaning).
 *
 * @param[in] node The node to check for leaning.
 *
 * @return If balanced or leaning to one of the sides.
 */
static balance_e avl_node_leaning(avl_node_s *node)
{
    int balance;

    /* Check for valid pointers */
    assert(node);

    balance = avl_node_get_balance(node);
    if (balance >= 1) {
        return RIGHT_HEAVY;
    } else if (balance <= -1) {
        return LEFT_HEAVY;
    } else {
        return BALANCED;
    }
}

/**
 * @brief Return if the given node is balanced.
 *
 * @param[in] node AVL Tree node to check for balance.
 * @param[out] side Side of the imbalance.
 */
static bool avl_node_balanced(avl_node_s *node, balance_e *side)
{
    int balance;

    /* Check for valid pointers */
    assert(node && side);

    balance = avl_node_get_balance(node);

    /* Assert AVL tree invariant */
    assert(balance <= 2 && balance >= -2);

    if (balance == -2) {
        *side = LEFT_HEAVY;
        return false;
    }
    if (balance == 2) {
        *side = RIGHT_HEAVY;
        return false;
    }

    *side = BALANCED;
    return true;
}

/**
 * @brief Update height in a node - for example after a rotation.
 *
 * @param node  The node tu update.
 */
static void avl_node_update_height(avl_node_s *node)
{
    node->height = MAX(avl_node_get_height(node->left), avl_node_get_height(node->right)) + 1;
}

/**
 * @brief Rotate the given AVL tree node to the right.
 *
 * @param[in] node AVL Tree node to rotate.
 */
static void avl_right_rotate(avl_node_s **node)
{
    avl_node_s *tmp;

    /* Check for pointers */
    assert(node && *node);

    tmp = (*node)->left;
    (*node)->left = tmp->right;
    tmp->right = *node;

    avl_node_update_height(*node);
    avl_node_update_height(tmp);
    *node = tmp;
}

/**
 * @brief Rotate the given AVL tree node to the left.
 *
 * @param[in] node AVL Tree node to rotate.
 */
static void avl_left_rotate(avl_node_s **node)
{
    avl_node_s *tmp;

    /* Check for pointers */
    assert(node && *node);

    tmp = (*node)->right;
    (*node)->right = tmp->left;
    tmp->left = *node;

    avl_node_update_height(*node);
    avl_node_update_height(tmp);
    *node = tmp;
}

/**
 * @brief Rebalance the given node. Does nothing if already balanced.
 *
 * @param[in] node AVL Tree node to rebalance.
 */
static void avl_rebalance(avl_node_s **node)
{
    balance_e parent, child;

    /* Check for pointers */
    assert(node && *node);

    avl_node_update_height(*node);

    if (avl_node_balanced(*node, &parent)) {
        /* Tree is balanced and does not need rebalancing */
        return;
    }

    if (parent == LEFT_HEAVY) {
        /* Parent is imbalanced on the left */
        child = avl_node_leaning((*node)->left);
        if (child == RIGHT_HEAVY) {
            /* Case LR solve by left and right rotation */
            avl_left_rotate(&(*node)->left);
            avl_right_rotate(node);

        } else {
            /* Case LL solve by right rotation */
            avl_right_rotate(node);
        }
    } else {
        /* Parent is imbalanced on the right */
        child = avl_node_leaning((*node)->right);

        if (child == LEFT_HEAVY) {
            /* Case RL solve by right and left rotation */
            avl_right_rotate(&(*node)->right);
            avl_left_rotate(node);
        } else {
            /* Case RR solve by left rotation */
            avl_left_rotate(node);
        }
    }
}

void avl_init(avl_node_s **node)
{
    /* Check pointers */
    if (!node) {
        return;
    }
    if (!*node) {
        return;
    }
    *node = NULL;
}

void avl_insert(avl_node_s **node, char *key, void *value)
{
    avl_node_s *new = NULL;
    int cmp;

    if (!*node) {
        /* Node with given key not found - Allocate new node */
        new = malloc(sizeof *new);
        new->key = key;
        new->value = value;
        new->left = NULL;
        new->right = NULL;
        new->height = 0;
        *node = new;
    }

    cmp = strcmp(key, (*node)->key);
    if (!cmp) {
        /* Update existing entry */
        (*node)->value = value;
    } else if (cmp < 0) {
        /* Continue in the left child */
        avl_insert(&(*node)->left, key, value);
        (*node)->height++;
    } else {
        /* Continue in the right child */
        avl_insert(&(*node)->right, key, value);
        (*node)->height++;
    }

    avl_rebalance(node);
}

bool avl_search(avl_node_s *node, const char *key, void **value)
{
    int cmp;

    if (!node) {
        /* Empty tree */
        if (value) {
            *value = NULL;
        }
        return false;
    }

    cmp = strcmp(key, node->key);
    if (!cmp) {
        /* Matching key found */
        if (value) {
            *value = node->value;
        }
        return true;
    } else if (cmp < 0) {
        /* Search in left sibling */
        return avl_search(node->left, key, value);
    } else {
        /* Search in right sibling */
        return avl_search(node->right, key, value);
    }
}

/**
 * @brief Replace the @p target by the rightmost node in @p node. Used for deleting a node.
 *
 * @param target The node to replace by the rightmost.
 * @param node AVL Tree where to find the rightmost node in.
 */
static void avl_replace_by_rightmost(avl_node_s *target, avl_node_s **node)
{
    avl_node_s *left = NULL;

    if (!(*node)->right) {
        /* Found the rightmost node - replace it */
        target->key = strdup((*node)->key);
        target->value = (*node)->value;
        target->height--;

        /* Link the left subtree of the rightmost node to the parent */
        left = (*node)->left;
        FREE((*node)->key);
        FREE(*node);
        *node = left;
        return;
    }
    avl_replace_by_rightmost(target, &(*node)->right);
}

bool avl_delete(avl_node_s **node, const char *key)
{
    avl_node_s *tmp = NULL;
    bool ret;
    int cmp;

    /* Check for pointers */
    if (!node || !*node) {
        return false;
    }

    cmp = strcmp(key, (*node)->key);
    if (cmp < 0) {
        /* Search in left subtree */
        if ((ret = avl_delete(&(*node)->left, key))) {
            (*node)->height--;
            avl_rebalance(node);
        }
        return ret;
    } else if (cmp > 0) {
        /* Search in right subtree */
        if ((ret = avl_delete(&(*node)->right, key))) {
            (*node)->height--;
            avl_rebalance(node);
        }
        return ret;
    } else {
        /* Found the node to be removed */

        if (!(*node)->left && !(*node)->right) {
            /* Terminal node */
            FREE((*node)->key);
            FREE(*node);
            *node = NULL;
        }

        else if ((*node)->left && !(*node)->right) {
            /* Has only left child */
            tmp = (*node)->left;
            FREE((*node)->key);
            FREE(*node);
            *node = tmp;
        }

        else if ((*node)->right && !(*node)->left) {
            /* Has only right child */
            tmp = (*node)->right;
            FREE((*node)->key);
            FREE(*node);
            *node = tmp;
        }

        else {
            /* Has both children */
            avl_replace_by_rightmost(*node, &(*node)->left);
        }

        return true;
    }
}

void avl_destroy(avl_node_s **node)
{

    /* Check for pointers */
    if (!node || !*node) {
        return;
    }

    while (*node) {
        avl_delete(node, (*node)->key);
    }
}
