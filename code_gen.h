/**
 * @file code_gen.h
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for code generator.
 */

#include <stdio.h>
#include <string.h>

#include "token_stack.h"

/**
 * @brief Generates code for the beginning of the program.
 */
void gen_prog_start(void);

/**
 * @brief Generates start of the function. Creates return values. Local variables for parameters.
 *
 * @param[in] func_name Name of the function used for label.
 * @param[in] in_param Stack of all the parameters of the function.
 * @param[in] no_returns Number of return parameters of the function.
 */
void gen_func_start(const char *func_name, tstack_s *in_param, unsigned no_returns);

/**
 * @brief Generates end of the function. Pushes return values into TF. Returns.
 *
 * @param[in] return_vals Stack of all the return values to push onto TF.
 */
void gen_func_end(tstack_s *return_vals);

/**
 * @brief Generates a conditional jump to else label if the content of GF@%tmp1 is a boolean@false.
 *
 * @return The label_number to be used for other generate calls.
 */
unsigned gen_jump_else(void);

/**
 * @brief Generate label for the else branch of an if-else statement. Also generates unconditional
 * jump to end of the if-else for the end of the if branch.
 *
 * @param[in] label_number The label_number returned by gen_jump_else call in the same if-else
 * statement.
 */
void gen_else_label(unsigned label_number);

/**
 * @brief Generates the label for the end of the if-else-statement for the if branch to jump after
 * finishing.
 *
 * @param label_number The label_number returned by gen_jump_else call in the same if-else
 * statement.
 */
void gen_if_end(unsigned label_number);

/**
 * @brief Generates the label for beginning for a while loop to jump after each iteration.
 * Condition follows this label.
 *
 * @return The label_number to be used for other generate calls.
 */
unsigned gen_while_label(void);

/**
 * @brief Generates a conditional jump to the end of the while statement.
 *
 * @param[in] label_number The label_number returned by gen_while_label call in the same while
 * statement.
 */
void gen_while_jump_end(unsigned label_number);

/**
 * @brief Generates label to jump after the end of the while statement.
 *
 * @param[in] label_number The label_number returned by gen_while_label call in the same while
 * statement.
 */
void gen_while_end_label(unsigned label_number);

/**
 * @brief Jumps to the beggining lable of while
 * 
 * @param[in] label_number The label_number returned by gen_while_label call in the same while
 * statement.
 */
void gen_while_jump_loop(unsigned label_number);

/**
 * @brief Generate code for a single operand in expression.
 *
 * @param[in] token The operand to generate.
 */
void gen_expr_operand(T_token *token);

/**
 * @brief Generate code for a single operator application in expression.
 *
 * @param[in] token The operator to generate.
 */
void gen_expr_operator(T_token *token);

/**
 * @brief Generates code to prepare expression result in GF@%tmp1.
 */
void gen_expr_cond(void);

/**
 * @brief Generates variadic write macro.
 *
 * @param[in] in_params Stack of paramters to print.
 */
void gen_write(tstack_s *in_params);

/**
 * @brief Add a function call to generate.
 *
 * @param function Pointer to the function token in global frame.
 * @param params_in The input parameters of the function. Will be freed.
 */
void gen_call_insert(T_token *function, tstack_s *params_in);

/**
 * @brief Generate all collected functions calls throughout the program.
 */
void gen_function_call_list(void);
