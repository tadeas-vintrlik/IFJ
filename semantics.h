/**
 * @file semantics.h
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @author Krystof Albrecht <xalbre05@stud.fit.vutbr.cz>
 * @brief Header for semantic functions mostly for type checks.
 */

#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include "symtable.h"
#include "token_stack.h"
#include <stdio.h>

void print_unexpected_token(T_token *bad_token, token_type expected_type, char *expected_content);

/**
 * @brief Check if token types are compatible. Integer is a subset of number.
 *
 * @param first First type.
 * @param second  Second type.
 * @return true Token types are compatible.
 * @return false Token types are not compatible.
 */
bool sem_check_type_compatible(symbol_type_e first, symbol_type_e second);

/**
 * @brief Checks if function call has compatible paramaters.
 *
 * @note This function is usefull for checking of call parameters.
 *
 * @param[in] function The function token to check for parmeters.
 * @param[in] call_params Stack with parameters of the call.
 * @param[in] line Line of the call to generate a helpful error.
 * @param[out] rc Return code to set.
 *
 * @return true All tokens have compatible types.
 * @return false Some tokens don't have compatible type.
 */
bool sem_call_types_compatible(T_token *function, tstack_s *call_params, unsigned line, rc_e *rc);

/**
 * @brief Check if called function was properly defined or declared.
 *
 * @param[in] token The input token to check if it was a defined or declared function.
 * @param[in] symtable The symtable where to find the declaration or definition.
 * @param[out] function The definition or declaration found in symtable.
 * @param[out] rc Return code to set.
 * @return true Function call was proper and the function name was decalred.
 * @return false The function name was not declared.
 */
bool sem_check_call_function(T_token *token, symtable_s *symtable, T_token **function, rc_e *rc);

/**
 * @brief Check if function was not redeclared.
 *
 * @param[in] token The token with the function name.
 * @param[in] symtable The symtable where to check for redeclaration.
 * @param[out] rc Return code to set.
 * @return true Function was not redeclared.
 * @return false Function was redeclared.
 */
bool sem_check_redecl(T_token *token, symtable_s *symtable, rc_e *rc);

/**
 * @brief Check if function was not redefined.
 *
 * @param[in] token The token with the function name.
 * @param[in] symtable The symtable where to check for redefinition.
 * @param[out] function The function token found in symtable will contain declaration.
 * @param[out] rc Return code to set.
 * @return true The function was not redefined.
 * @return false The function was redefined or defined after declaration (function->defined will be
 * false, this return is not an error)
 */
bool sem_check_redef(T_token *token, symtable_s *symtable, T_token **function, rc_e *rc);

/**
 * @brief Check if function declaration and definition parameter types match.
 *
 * @param[in] token The token with the fun_info from symtable.
 * @param[in] in_params The stack of collected in parameters. WILL BE FREED IF MATCHING.
 * @param[out] rc Return code to set.
 * @return true The definition was ok.
 * @return false The definition did not match the declaration.
 */
bool sem_check_decl_def_params(T_token *token, tstack_s *in_params, rc_e *rc);

/**
 * @brief Check if function declaration and definition return types match.
 *
 * @param[in] token The token with the fun_info from symtable.
 * @param[in] out_params The stack of collected return types. WILL BE FREED IF MATCHING.
 * @param[out] rc Return code to set.
 * @return true The definition was ok.
 * @return false The definition did not match the declaration.
 */
bool sem_check_decl_def_returns(T_token *token, tstack_s *out_params, rc_e *rc);

/**
 * @brief Check if identifier was declared.
 *
 * @param[in] token The identifier to check.
 * @param[in] symtable The symtable where to find the identifier.
 * @param[out] identifier The token found with this identifier.
 * @param[out] rc Return code to set.
 * @return true Identifier was declared
 * @return false Identifier was not declared.
 */
bool sem_check_id_decl(T_token *token, symtable_s *symtable, T_token **identifier, rc_e *rc);

/**
 * @brief Check if identifier is not being redeclared
 *
 * @param[in] token The identifier to check.
 * @param[in] symtable The symtable where to find the identifier.
 * @param[out] rc Return code to set.
 * @return true The identifier was NOT being redecalred. OK.
 * @return false The identifier was redeclared. ERROR.
 */
bool sem_check_id_redecl(T_token *token, symtable_s *symtable, rc_e *rc);

/**
 * @brief Check if the expression passed to string length has correct type.
 *
 * @param[in] token The expression passed to string length.
 * @param[out] rc Return code to set.
 * @return true Called with correct expression.
 * @return false Called with incorrect expression.
 */
bool sem_check_string_length(T_token *token, rc_e *rc);

/**
 * @brief Check if two operand, one operator expression is semantically correct.
 *
 * @param[in] first First operand. Non-terminal.
 * @param[in] second The operator.
 * @param[in] third Second operand. Non-terminal. Type of this operand will be set to the type of
 * the result.
 * @param[out] rc Return code to set.
 * @return true The expression was correct.
 * @return false The expression was incorrect.
 */
bool sem_check_expr_type(T_token *first, T_token *second, T_token *third, rc_e *rc);

/**
 * @brief Checks if it is semantically correct to assign from @p second to @p first. That means the
 *following: 1) @p first is the same length or shorter than @p second 2) the types for each token
 *are compatible number in @p first and integer in @p second are compatible as integer is a subset
 *of number but the other way around is not legal.
 *
 * @param[in] first First stack. The variables to assign to.
 * @param[in] second Second stack. The function out parameters.
 * @param[in] line Line of the assignment to generate a helpful error.
 * @param[out] rc Return code to set.
 *
 * @return true All tokens have the same type.
 * @return false Some tokens don't have the same type.
 */
bool sem_check_call_assign(tstack_s *first, tstack_s *second, unsigned line, rc_e *rc);

#endif /* _SEMANTICS_H_ */
