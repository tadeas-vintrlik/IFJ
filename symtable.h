/**
 * @file symtable.h
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for table of symbols.
 */

#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "avl.h"
#include "common.h"
#include "sll.h"
#include "token_stack.h"

/**
 * @brief Structure for the table of symbols.
 */
typedef struct symtable {
    sll_s *frames;
    avl_node_s *global;
} symtable_s;

/**
 * @brief Initialization of the table of symbols structure.
 *
 * @param[in] symtable The table of symbols to initialize.
 */
void symtable_init(symtable_s *symtable);

/**
 * @brief Search for a token among all the frames. Excluding global which is meant for functions.
 * only.
 *
 * @param[in] symtable The table of symbols to initialize.
 * @param[in] key The key of the token to find.
 * @param[out] token If the token was found it will be placed here. If @p token is NULL the token
 * will not be stored. if it was nout found NULL will be stored.
 *
 * @return True if token was found. False if not found.
 */
bool symtable_search_all(const symtable_s *symtable, const char *key, T_token **token);

/**
 * @brief Search for a token in the top frame. Always returns false if there is no local frame.
 *
 * @param[in] symtable The table of symbols to initialize.
 * @param[in] key The key of the token to find.
 * @param[out] token If the token was found it will be placed here. If @p token is NULL the token
 * will not be stored. if it was nout found NULL will be stored.
 *
 * @return True if token was found. False if not found.
 */
bool symtable_search_top(const symtable_s *symtable, const char *key, T_token **token);

/**
 * @brief Search for a token in the global frame. Global frame is meant for functions only.
 *
 * @param[in] symtable The table of symbols to initialize.
 * @param[in] key The key of the token to find.
 * @param[out] token If the token was found it will be placed here. If @p token is NULL the token
 * will not be stored. if it was nout found NULL will be stored.
 *
 * @return True if token was found. False if not found.
 */
bool symtable_search_global(const symtable_s *symtable, const char *key, T_token **token);

/**
 * @brief Creates a new top local frame.
 *
 * @param[in] symtable The table of symbols where to create the new frame.
 */
void symtable_new_frame(symtable_s *symtable);

/**
 * @brief Pops the top local frame. If there was no frame it does nothing.
 *
 * @param[in] symtable The table of symbols where to pop the frame.
 */
void symtable_pop_frame(symtable_s *symtable);

/**
 * @brief Returns the number of frames in the symtable.
 *
 * @param[in] symtable The table of symbols where count frame depth.
 *
 * @return The depth of symtable frames.
 */
unsigned symtable_frames_depth(const symtable_s *symtable);

/**
 * @brief Check if there are any local frames in the symtable.
 *
 * @param[in,out] symtable The table of symbols where to check for local frames.
 *
 * @return True if there are no frames. False otherwise.
 */
bool symtable_frames_empty(const symtable_s *symtable);

/**
 * @brief Insert a new token in the top local frame. If there is no top frame it does nothing.
 * Always check if local frames are not empty. Meant for variables only.
 *
 * @param[in,out] symtable The table of symbols where to insert a new token.
 * @param[out] token The token to insert.
 */
void symtable_insert_token_top(symtable_s *symtable, T_token *token);

/**
 * @brief Insert a new token in the global frame. Meant for functions only.
 *
 * @param[in,out] symtable The table of symbols where to insert a new token.
 * @param[out] token The token to insert.
 */
void symtable_insert_token_global(symtable_s *symtable, T_token *token);

/**
 * @brief Destructor of the table of symbols structure.
 *
 * @param symtable The table of symbols to destory.
 */
void symtable_destroy(symtable_s *symtable);

#endif /* _SYMTABLE_H */
