/**
 * @file semantics.h
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @author Krystof Albrecht <xalbre05@stud.fit.vutbr.cz>
 * @brief Header for semantic functions mostly for type checks.
 */

#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include "sll.h"
#include "token_stack.h"
#include <stdio.h>

void print_unexpected_token(T_token *bad_token, token_type expected_type, char *expected_content);

/**
 * @brief Checks if it is semantically correct to assign from @p second to @p first. That means the
 *following: 1) @p first is the same length or shorter than @p second 2) the types for each token
 *are compatible number in @p first and integer in @p second are compatible as integer is a subset
 *of number but the other way around is not legal.
 *
 * @note This can be used for assignment checks but NOT for call or definition checks.
 * See token_list_type_identical below for that.
 *
 * @param[in] first First stack.
 * @param[in] second Second stack.
 *
 * @return true All tokens have the same type.
 * @return false Some tokens don't have the same type.
 */
bool token_list_type_assignable(tstack_s *first, tstack_s *second);

/**
 * @brief Checks if function call has compatible paramaters.
 *
 * @note This function is usefull for checking of call parameters.
 *
 * @param[in] function The function token to check for parmeters.
 * @param[in] call_params Stack with parameters of the call.
 * @param[out] rc Return code to set.
 *
 * @return true All tokens have compatible types.
 * @return false Some tokens don't have compatible type.
 */
bool sem_call_types_compatible(T_token *function, tstack_s *call_params, rc_e *rc);


/**
 * @brief Checks if all tokens in two stacks of tokens have the same type. Will change the activity
 * of both lists.
 *
 * @param[in] first First stack.
 * @param[in] second Second stack.
 *
 * @return true All tokens have the same type.
 * @return false Some tokens don't have the same type.
 */
bool token_list_types_identical(tstack_s *first, tstack_s *second);

#endif /* _SEMANTICS_H_ */
