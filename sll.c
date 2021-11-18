/**
 * @file sll.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of Single Linked List ADT.
 */

#include "sll.h"

void sll_init(sll_s *list)
{
    if (!list) {
        return;
    }
    list->head = NULL;
    list->active = NULL;
}

void sll_insert_head(sll_s *list, void *value)
{
    sll_elem_s *new, *tmp;

    if (!list) {
        return;
    }
    new = malloc(sizeof *new);
    ALLOC_CHECK(new);

    /* Link as new head */
    tmp = list->head;
    list->head = new;

    /* Set element attributes */
    new->value = value;
    new->next = tmp;
}

void sll_delete_head(sll_s *list, bool destroy)
{
    sll_elem_s *deleted, *tmp = NULL;

    if (!list) {
        return;
    }

    deleted = list->head;
    if (deleted == list->active) {
        /* If deleting active lose activity */
        list->active = NULL;
    }
    if (deleted) {
        /* If there was a head free it */
        tmp = deleted->next;
        if (destroy) {
            FREE(deleted->value);
        }
    }

    FREE(deleted); /* Free on null does nothing */
    list->head = tmp;
}

void sll_destroy(sll_s *list, bool destory)
{
    if (!list) {
        return;
    }

    while (list->head) {
        sll_delete_head(list, destory);
    }

    /* Set the default values to the list */
    sll_init(list);
}

void sll_activate(sll_s *list)
{
    if (!list) {
        return;
    }
    list->active = list->head;
}

bool sll_is_empty(const sll_s *list) { return list->head == NULL; }

bool sll_is_active(const sll_s *list) { return list->active != NULL; }

void sll_next(sll_s *list)
{
    if (!list) {
        return;
    }

    if (!sll_is_active(list)) {
        return;
    }

    list->active = list->active->next;
}

void sll_insert_after(sll_s *list, void *value)
{
    sll_elem_s *new;

    if (!list) {
        return;
    }

    if (!sll_is_active(list)) {
        return;
    }
    new = malloc(sizeof *new);
    ALLOC_CHECK(new);
    new->value = value;
    new->next = list->active->next;

    list->active->next = new;
}

void sll_delete_after(sll_s *list, bool destroy)
{
    sll_elem_s *tmp;

    if (!list) {
        return;
    }
    if (!sll_is_active(list)) {
        return;
    }
    if (!list->active->next) {
        return;
    }

    tmp = list->active->next->next;
    if (destroy) {
        FREE(list->active->next->value);
    }
    FREE(list->active->next);
    list->active->next = tmp;
}

void *sll_get_head(const sll_s *list)
{
    if (!list) {
        return NULL;
    }
    if (!list->head) {
        return NULL;
    }
    return list->head->value;
}

void *sll_get_active(const sll_s *list)
{
    if (!list) {
        return NULL;
    }
    if (!list->active) {
        return NULL;
    }

    return list->active->value;
}

void *sll_get_after(const sll_s *list)
{
    if (!list) {
        return NULL;
    }
    if (!list->active) {
        return NULL;
    }
    if (!list->active->next) {
        return NULL;
    }

    return list->active->next->value;
}

unsigned sll_get_length(const sll_s *list)
{
    unsigned len = 0;
    sll_elem_s *elem;

    if (!list) {
        return len;
    }

    elem = list->head;
    while (elem) {
        len++;
        elem = elem->next;
    }

    return len;
}

void sll_insert_last(sll_s *list, void *value)
{
    sll_elem_s *old_last;
    sll_elem_s *new;

    if (!list) {
        return;
    }

    new = malloc(sizeof *new);
    ALLOC_CHECK(new);
    new->value = value;
    new->next = NULL;

    old_last = list->head;
    if (!old_last) {
        list->head = new;
        return;
    }

    while (old_last->next) {
        old_last = old_last->next;
    }
    old_last->next = new;
}

void sll_delete_last(sll_s *list, bool destroy)
{
    sll_elem_s *old_last;

    if (!list) {
        return;
    }

    old_last = list->head;
    if (!old_last) {
        return;
    }

    while (old_last->next && old_last->next->next) {
        old_last = old_last->next;
    }

    if (old_last == list->head && !list->head->next) {
        /* When only one it is the same as deleting head */
        sll_delete_head(list, destroy);
    } else {
        if (destroy) {
            FREE(old_last->next->value);
        }
        FREE(old_last->next);
    }

}

void *sll_get_last(const sll_s *list)
{
    sll_elem_s *last;

    if (!list) {
        return NULL;
    }

    last = list->head;
    if (!last) {
        return NULL;
    }

    while (last->next) {
        last = last->next;
    }

    return last->value;
}

