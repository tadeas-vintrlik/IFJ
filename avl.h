/**
 * @file avl.h
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for AVL Binary Trees.
 */
#ifndef _AVL_H_
#define _AVL_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief AVL Tree node structure.
 */
typedef struct avl_node {
    char *key;
    void *value;
    int height;
    struct avl_node *left;
    struct avl_node *right;
} avl_node_s;

/**
 * @brief Initialization of the root node of the AVL Tree sturcture.
 *
 * @param[in/out] node The tree node to be initialized.
 */
void avl_init(avl_node_s **node);

/**
 * @brief Insertion into the AVL Tree sturcture.
 *
 * @param[in/out] node The root node of the tree into which to insert.
 * @param[in] key The key that will be used for searching.
 * @param[in] value A generic pointer to the value to insert.
 */
void avl_insert(avl_node_s **node, char *key, void *value);

/**
 * @brief Searching inside the AVL Tree sturcture.
 *
 * @param[in/out] node The root node of the tree where to search.
 * @param[in] key The key that will be used for searching.
 * @param[out] value A generic pointer where to store the found value. NULL set if not found. If @p
 * value is NULL nothing is stored.
 *
 * @return true if found, false otherwise.
 */
bool avl_search(avl_node_s *node, const char *key, void **value);

/**
 * @brief Deletion of the node inside the AVL Tree sturcture.
 *
 * @note If the node with the given @p key does not exist nothing happens.
 *
 * @param[in/out] node The root node of the tree where to delete.
 * @param[in] key The key that will be used for searching.
 *
 * @return True if value was deleted. False if not found.
 */
bool avl_delete(avl_node_s **node, const char *key);

/**
 * @brief Destructor for the AVL Tree structure.
 *
 * @note Free will be called on value pointers in all nodes of the tree.
 *
 * @param[in/out] node The root of the tree to destroy.
 */
void avl_destroy(avl_node_s **node);

#endif
