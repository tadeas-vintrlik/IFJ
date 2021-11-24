/**
 * @file code_gen.h
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for code generator.
 */

#include <stdio.h>

#include "token_stack.h"

/**
 * @brief Generates code for the beginning of the program.
 */
void gen_prog_start(void);

/**
 * @brief Generates start of the function. Creates return values. Local variables for parameters.
 *
 * @param func_name Name of the function used for label.
 * @param in_param Stack of all the parameters of the function.
 * @param no_returns Number of return parameters of the function.
 */
void gen_func_start(const char *func_name, tstack_s *in_param, unsigned no_returns);

/**
 * @brief Generates end of the function. Pushes return values into TF. Returns.
 *
 * @param return_vals Stack of all the return values to push onto TF.
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
 * @param label_number The label_number returned by gen_jump_else call in the same if-else
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
 * @param label_number The label_number returned by gen_while_label call in the same while
 * statement.
 */
void gen_while_jump_end(unsigned label_number);

/**
 * @brief Generates label to jump after the end of the while statement.
 *
 * @param label_number The label_number returned by gen_while_label call in the same while
 * statement.
 */
void gen_while_end_label(unsigned label_number);

/**
 * @brief Generates call to a function passes the parameters using TF.
 *
 * @param func_name The name of the function to call.
 * @param in_params The stack of the parameters to call function with.
 */
void gen_func_call(const char *func_name, tstack_s *in_params);
