/**
 * @file sll.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of Single Linked List ADT.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "sll.h"

void sll_init(sll_s *list)
{
    if (!list) {
        return;
    }
    list->head = NULL;
    list->active = NULL;
}

void sll_insert_head(sll_s *list, void *data)
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
    new->data = data;
    new->next = tmp;
}

void sll_delete_head(sll_s *list)
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
        FREE(deleted->data);
    }

    FREE(deleted); /* Free on null does nothing */
    list->head = tmp;
}

void sll_destroy(sll_s *list)
{
    if (!list) {
        return;
    }

    while (list->head) {
        sll_delete_head(list);
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

bool sll_is_empty(sll_s *list) { return list->head == NULL; }

bool sll_is_active(sll_s *list) { return list->active != NULL; }

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

void sll_insert_after(sll_s *list, void *data)
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
    new->data = data;
    new->next = list->active->next;

    list->active->next = new;
}

void sll_delete_after(sll_s *list)
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
    FREE(list->active->next->data);
    FREE(list->active->next);
    list->active->next = tmp;
}

void *sll_get_head(sll_s *list)
{
    if (!list) {
        return NULL;
    }
    if (!list->head) {
        return NULL;
    }
    return list->head->data;
}

void *sll_get_active(sll_s *list)
{
    if (!list) {
        return NULL;
    }
    if (!list->active) {
        return NULL;
    }

    return list->active->data;
}

void *sll_get_after(sll_s *list)
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

    return list->active->next->data;
}
