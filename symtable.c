/**
 * @file symtable.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of table of symbols.
 */

#include "symtable.h"
#include <stdlib.h>

static void data_destroy(void *data) { token_destroy((T_token *)data); }

void symtable_insert_token_global(symtable_s *symtable, T_token *token);

static void populate_type_list(tstack_s *type_list, symbol_type_e types[], unsigned count)
{
    for (unsigned i = 0; i < count; i++) {
        T_token *token = malloc(sizeof(T_token));
        ALLOC_CHECK(token);
        token_init(token);

        token->symbol_type = types[i];

        sll_insert_last(type_list, token);
    }
}

static T_token *create_token(const char *key)
{
    T_token *new;
    new = malloc(sizeof *new);
    ALLOC_CHECK(new);

    token_init(new);

    for (unsigned i = 0; key[i]; i++) {
        ds_add_char(new->value, key[i]);
    }

    new->fun_info = malloc(sizeof(function_info_s));
    ALLOC_CHECK(new->fun_info);
    function_info_init(new->fun_info);

    if (!strcmp("reads", key)) {
        symbol_type_e out_types[] = { SYM_TYPE_STRING };
        populate_type_list(new->fun_info->out_params, out_types, 1);

    } else if (!strcmp("readn", key)) {
        symbol_type_e out_types[] = { SYM_TYPE_NUMBER };
        populate_type_list(new->fun_info->out_params, out_types, 1);

    } else if (!strcmp("readi", key)) {
        symbol_type_e out_types[] = { SYM_TYPE_INT };
        populate_type_list(new->fun_info->out_params, out_types, 1);

    } else if (!strcmp("tointeger", key)) {
        symbol_type_e in_types[] = { SYM_TYPE_NUMBER };
        symbol_type_e out_types[] = { SYM_TYPE_INT };
        populate_type_list(new->fun_info->in_params, in_types, 1);
        populate_type_list(new->fun_info->out_params, out_types, 1);

    } else if (!strcmp("substr", key)) {
        // TODO: INT should be NUMBER, but implicit conversion is not implemented yet
        symbol_type_e in_types[] = { SYM_TYPE_STRING, SYM_TYPE_INT, SYM_TYPE_INT };
        symbol_type_e out_types[] = { SYM_TYPE_STRING };
        populate_type_list(new->fun_info->in_params, in_types, 3);
        populate_type_list(new->fun_info->out_params, out_types, 1);

    } else if (!strcmp("ord", key)) {
        symbol_type_e in_types[] = { SYM_TYPE_STRING, SYM_TYPE_INT };
        symbol_type_e out_types[] = { SYM_TYPE_INT };
        populate_type_list(new->fun_info->in_params, in_types, 2);
        populate_type_list(new->fun_info->out_params, out_types, 1);

    } else if (!strcmp("chr", key)) {
        symbol_type_e in_types[] = { SYM_TYPE_INT };
        symbol_type_e out_types[] = { SYM_TYPE_STRING };
        populate_type_list(new->fun_info->in_params, in_types, 1);
        populate_type_list(new->fun_info->out_params, out_types, 1);
    }

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
