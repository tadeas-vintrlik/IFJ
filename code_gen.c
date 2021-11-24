/**
 * @file code_gen.c
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of code generator.
 */

 #include "code_gen.h"

 static unsigned counter = 0;

void gen_prog_start(void)
{
    puts(".IFJcode21");
    puts("JUMP $-main");
}

void gen_func_start(const char *func_name, tstack_s *in_params, unsigned no_returns)
{
    T_token *token;
    unsigned i;

    /* Create label for calls to jump to and PUSH TF to LF */
    printf("LABEL $-%s\n", func_name);
    puts("PUSHFRAME");

    /* Create variables for return values */
    for (i = 0; i < no_returns; i++) {
        printf("DEFVAR LF@%%retval%d\n", no_returns);
        printf("MOVE LF@%%retval%d nil@nil\n", no_returns);
    }

    /* Move input parameters to their actual variable names */
    i = 0;
    while (!tstack_empty(in_params)) {
        token = tstack_top(in_params);
        tstack_pop(in_params, false);
        printf("DEFVAR LF@%s", token->value->content);
        printf("MOVE LF@%s LF@%%p%d", token->value->content, i);
        i++;
        FREE(token);
    }
}

/**
 * @brief Helper function for gen_func_end. Moves all the return values to correct out parameters in LF.
 *
 * @param return_vals The stack of return value tokens the function should return.
 */
static void gen_push_arg(tstack_s *return_vals)
{
    T_token *token;
    unsigned i = 0;

    /* Move all return values to correct variables */
    while (!tstack_empty(return_vals)) {
        token = tstack_top(return_vals);
        tstack_pop(return_vals, false);
        switch (token->type) {
        case TOKEN_ID:
            printf("MOVE LF@%%retval%d LF@%s\n", i, token->value->content);
            break;
        case TOKEN_INT:
            printf("MOVE LF@%%retval%d int@%s\n", i, token->value->content);
            break;
        case TOKEN_NUMBER:
            printf("MOVE LF@%%retval%d float@%s\n", i, token->value->content);
            break;
        case TOKEN_STRING:
            printf("MOVE LF@%%retval%d string@%s\n", i, token->value->content);
            break;
        default:
            /* Should not happen */
            ERR_MSG("Unexpected type of return value: ", token->line);
            fprintf(stderr, "%d\n", token->type);
            break;
        }

        i++;
        FREE(token);
    }
}

void gen_func_end(tstack_s *return_vals)
{
    /* Move correct return values variables in LF */
    gen_push_arg(return_vals);

    /* Push current LF as TF for caller to pop the return values and return to addres */
    puts("POPFRAME");
    puts("RETURN");
}

static void gen_pop_arg();
static void gen_push_ret();
static void gen_pop_ret();

unsigned gen_jump_else(void)
{
    unsigned ret;
    printf("JUMPIFNEQ $-else%d GF@%%expr bool@true\n", counter);
    ret = counter;
    counter++;
    return ret;
}

void gen_else_label(unsigned label_number)
{
    printf("JUMP $-end%d\n", label_number);
    printf("LABEL $-else%d\n", label_number);
}

void gen_if_end(unsigned label_number)
{
    printf("LABEL $-end%d\n", label_number);
}

unsigned gen_while_label(void)
{
    unsigned ret;
    printf("LABEL $-while%d\n", counter);
    ret = counter;
    counter++;
    return ret;
}

void gen_while_jump_end(unsigned label_number)
{
    printf("JUMPIFNEQ $-end%d GF@%%expr bool@true\n", label_number);
}

void gen_while_end_label(unsigned label_number)
{
    printf("LABEL $-end%d\n", label_number);
}
