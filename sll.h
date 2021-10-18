/**
 * @file sll.h
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for Single Linked List ADT.
 */
#ifndef _SLL_H_
#define _SLL_H_

#include <stdbool.h>

/**
 * @brief Single Linked List element structure.
 */
typedef struct sll_elem {
    void *data;
    struct sll_elem *next;
} sll_elem_s;

/**
 * @brief Single Linked List structure.
 */
typedef struct sll {
    sll_elem_s *head;
    sll_elem_s *active;
} sll_s;

/**
 * @brief Initialization of the Singe Linked List sturcture.
 *
 * @param[in/out] list The list to be initialized.
 */
void sll_init(sll_s *list);

/**
 * @brief Insert element into the list as new first element.
 *
 * @param[in/out] list The list where to insert the new element.
 * @param[in] data A generic pointer to the data to insert. Will be freed upon sll_destroy.
 */
void sll_insert_first(sll_s *list, void *data);

/**
 * @brief Delete the first element of the list.
 *
 * @note Does nothing when there was no first element. If first was active activity is lost.
 *
 * @param[in/out] list List where to remove the first element.
 */
void sll_delete_first(sll_s *list);

/**
 * @brief Destructor for the Single Linked List structure.
 *
 * @param[in/out] list
 */
void sll_destroy(sll_s *list);

/**
 * @brief Set active element to the first element of the list.
 *
 * @param[in/out] list The list where to set active element in.
 */
void sll_activate(sll_s *list);

/**
 * @brief Check if list is empty.
 *
 * @param[in] list List to check for emptiness.
 */
bool sll_is_empty(sll_s *list);

/**
 * @brief Check if list is active.
 *
 * @param[in] list List to check for activity.
 */
bool sll_is_active(sll_s *list);

/**
 * @brief Set the active element to the next element of the list.
 *
 * @note If the list was not active nothing will happen.
 * If there was no next element the list will turn inactive.
 * ALWAYS check for activity after calling this function.
 *
 * @param[in/out] list List where to change the active element.
 */
void sll_next(sll_s *list);

/**
 * @brief Insert a new element after the active element of the list.
 *
 * @note If the list was not active nothing will happen.
 *
 * @param[in/out] list List where to insert the new element.
 * @param[in] data A generic pointer to the new data.
 */
void sll_insert_after(sll_s *list, void *data);

/**
 * @brief Delete element after the active element in the list.
 *
 * @note If the list was not active or activity is on the last element noting will happen.
 *
 * @param[in/out] list List where to delete the element.
 */
void sll_delete_after(sll_s *list);

/**
 * @brief Get the data of first element.
 *
 * @return Gerenic pointer to the data of first element or NULL if empty.
 */
void *sll_get_first(sll_s *list);

/**
 * @brief Get the data of active element.
 *
 * @return Gerenic pointer to the data of active element or NULL if not active.
 */
void *sll_get_active(sll_s *list);

/**
 * @brief Get the the data of element after active.
 *
 * @return Gerenic pointer to the data element after active or NULL if not active or no next.
 */
void *sll_get_after(sll_s *list);

#endif
