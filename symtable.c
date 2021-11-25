/**
 * @file symtable.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of table of symbols.
 */

#include "symtable.h"
#include <stdlib.h>

static void data_destroy(void *data) { token_destroy((T_token *)data); }

void symtable_insert_token_global(symtable_s *symtable, T_token *token);

static T_token *create_token(const char *key)
{
    T_token *new;
    dynamic_string_s *ds;

    new = malloc(sizeof *new);
    ALLOC_CHECK(new);
    ds = malloc(sizeof *ds);
    ALLOC_CHECK(ds);

    ds_init(ds);
    for (unsigned i = 0; key[i]; i++) {
        ds_add_char(ds, key[i]);
    }
    ds_add_char(ds, '\0');
    new->value = ds;
    new->type = TOKEN_ID;
    new->line = -1;

    return new;
}

void symtable_init(symtable_s *symtable)
{
    sll_s *frames;
    avl_node_s *global;

    /* Initialize the frames list */
    frames = malloc(sizeof *frames);
    ALLOC_CHECK(frames);
    sll_init(frames);
    symtable->frames = frames;

    /* Initialize the global frame AVL tree */
    avl_init(&global);
    symtable->global = global;

    char *built_ins[] = { "reads", "readn", "readi", "write", "tointeger", "substr", "ord", "chr" };
    for (unsigned i = 0; i < 8; i++) {
        symtable_insert_token_global(symtable, create_token(built_ins[i]));
    }
}

bool symtable_search_all(const symtable_s *symtable, const char *key, T_token **token)
{
    bool found = false;

    if (!symtable) {
        return false;
    }

    /* Go through all the frames trying to find the key */
    sll_activate(symtable->frames);
    while (sll_is_active(symtable->frames)) {
        if ((found = avl_search(sll_get_active(symtable->frames), key, (void **)token))) {
            break;
        }
        sll_next(symtable->frames);
    }

    return found;
}

bool symtable_search_top(const symtable_s *symtable, const char *key, T_token **token)
{
    if (!symtable) {
        return false;
    }

    if (sll_is_empty(symtable->frames)) {
        return false;
    }

    return avl_search(sll_get_head(symtable->frames), key, (void **)token);
}

bool symtable_search_global(const symtable_s *symtable, const char *key, T_token **token)
{
    if (!symtable) {
        return false;
    }

    return avl_search(symtable->global, key, (void **)token);
}

void symtable_new_frame(symtable_s *symtable)
{
    if (!symtable) {
        return;
    }

    sll_insert_head(symtable->frames, NULL);
}

void symtable_pop_frame(symtable_s *symtable)
{
    avl_node_s *node;
    if (!symtable) {
        return;
    }

    node = sll_get_head(symtable->frames);
    avl_destroy(&node, data_destroy);
    sll_delete_head(symtable->frames, false);
}

unsigned symtable_frames_depth(const symtable_s *symtable)
{
    unsigned depth = 0;
    sll_elem_s *elem;

    if (!symtable) {
        return depth;
    }

    elem = symtable->frames->head;
    while (elem) {
        depth++;
        elem = elem->next;
    }

    return depth;
}

bool symtable_frames_empty(const symtable_s *symtable)
{
    if (!symtable) {
        return false;
    }

    return symtable_frames_depth(symtable) == 0;
}

void symtable_insert_token_top(symtable_s *symtable, T_token *token)
{
    avl_node_s *node;

    if (!symtable) {
        return;
    }

    if (sll_is_empty(symtable->frames)) {
        return;
    }

    node = sll_get_head(symtable->frames);
    avl_insert(&node, token->value->content, token);
    sll_delete_head(symtable->frames, false);
    sll_insert_head(symtable->frames, node);
}

void symtable_insert_token_global(symtable_s *symtable, T_token *token)
{
    if (!symtable) {
        return;
    }

    avl_insert(&symtable->global, token->value->content, token);
}

void symtable_destroy(symtable_s *symtable)
{
    avl_node_s *node;

    if (!symtable) {
        return;
    }

    sll_activate(symtable->frames);
    while (sll_is_active(symtable->frames)) {
        node = sll_get_head(symtable->frames);
        avl_destroy(&node, data_destroy);
        sll_delete_head(symtable->frames, false);
        sll_activate(symtable->frames);
    }
    FREE(symtable->frames);
    avl_destroy(&symtable->global, data_destroy);
}
