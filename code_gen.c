/**
 * @file code_gen.c
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of code generator.
 */

#include "code_gen.h"
#include "common.h"

static unsigned counter = 0;

static void generate_built_ins(void);

void gen_prog_start(void)
{
    puts(".IFJcode21");
    puts("JUMP $-main");
    puts("DEFVAR GF@%tmp1");
    puts("DEFVAR GF@%tmp2");
    generate_built_ins();
}

/**
 * @brief  Move input parameters to their actual variable names.
 *
 * @param in_params Stack of all the parameters of the function.
 */
static void gen_pop_arg(tstack_s *in_params)
{
    T_token *token;
    unsigned i;

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

void gen_func_start(const char *func_name, tstack_s *in_params, unsigned no_returns)
{
    unsigned i = 0;

    /* Create label for calls to jump to and PUSH TF to LF */
    printf("LABEL $-%s\n", func_name);
    puts("PUSHFRAME");

    /* Create variables for return values */
    for (i = 0; i < no_returns; i++) {
        printf("DEFVAR LF@%%retval%d\n", no_returns);
        printf("MOVE LF@%%retval%d nil@nil\n", no_returns);
    }
    gen_pop_arg(in_params);
}

/**
 * @brief Helper function for gen_func_end. Moves all the return values to correct out parameters in
 * LF.
 *
 * @param return_vals The stack of return value tokens the function should return.
 */
static void gen_push_ret(tstack_s *return_vals)
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
    gen_push_ret(return_vals);

    /* Push current LF as TF for caller to pop the return values and return to addres */
    puts("POPFRAME");
    puts("RETURN");
}

unsigned gen_jump_else(void)
{
    unsigned ret;
    printf("JUMPIFNEQ $-else%d GF@%%tmp1 bool@true\n", counter);
    ret = counter;
    counter++;
    return ret;
}

void gen_else_label(unsigned label_number)
{
    printf("JUMP $-end%d\n", label_number);
    printf("LABEL $-else%d\n", label_number);
}

void gen_if_end(unsigned label_number) { printf("LABEL $-end%d\n", label_number); }

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
    printf("JUMPIFNEQ $-end%d GF@%%tmp1 bool@true\n", label_number);
}

void gen_while_end_label(unsigned label_number) { printf("LABEL $-end%d\n", label_number); }

static void gen_push_arg(tstack_s *in_params)
{
    T_token *token;
    unsigned i = 0;

    /* Move all in paramters values to correct variables */
    while (!tstack_empty(in_params)) {
        token = tstack_top(in_params);
        tstack_pop(in_params, false);
        printf("DEFVAR TF@%%p%d\n", i);
        switch (token->type) {
        case TOKEN_ID:
            printf("MOVE TF@%%p%d LF@%s\n", i, token->value->content);
            break;
        case TOKEN_INT:
            printf("MOVE TF@%%p%d int@%s\n", i, token->value->content);
            break;
        case TOKEN_NUMBER:
            printf("MOVE TF@%%p%d float@%s\n", i, token->value->content);
            break;
        case TOKEN_STRING:
            printf("MOVE TF@%%p%d string@%s\n", i, token->value->content);
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

void gen_func_call(const char *func_name, tstack_s *in_params)
{
    puts("CREATEFRAME");
    gen_push_arg(in_params);
    printf("CALL $-%s", func_name);
}

void gen_expr_operand(T_token *token)
{
    switch (token->type) {
    case TOKEN_ID:
        printf("PUSHS %s\n", token->value->content);
        break;
    case TOKEN_INT:
        printf("PUSHS int@%s\n", token->value->content);
        break;
    case TOKEN_NUMBER:
        printf("PUSHS float@%s\n", token->value->content);
        break;
    case TOKEN_STRING:
        printf("PUSHS string@%s\n", token->value->content);
        break;
    default:
        /* Should not happen */
        ERR_MSG("Unexpected type of operand value: ", token->line);
        fprintf(stderr, "%d\n", token->type);
        break;
    }
}

void gen_expr_operator(T_token *token)
{
    switch (token->type) {
    case TOKEN_EQUAL:
        puts("EQS");
        break;
    case TOKEN_LESS_THAN:
        puts("LTS");
        break;
    case TOKEN_LESS_EQUAL_THAN:
        puts("POPS GF@%tmp2");
        puts("POPS GF@%tmp1");
        puts("PUSHS GF@%tmp1");
        puts("PUSHS GF@%tmp2");
        puts("PUSHS GF@%tmp1");
        puts("PUSHS GF@%tmp2");
        puts("LTS");
        puts("POPS GF@%tmp1");
        puts("EQS");
        puts("PUSHS GF@%tmp1");
        puts("ORS");
        break;
    case TOKEN_GREATER_THAN:
        puts("GTS");
        break;
    case TOKEN_GREATER_EQUAL_THAN:
        puts("POPS GF@%tmp2");
        puts("POPS GF@%tmp1");
        puts("PUSHS GF@%tmp1");
        puts("PUSHS GF@%tmp2");
        puts("PUSHS GF@%tmp1");
        puts("PUSHS GF@%tmp2");
        puts("GTS");
        puts("POPS GF@%tmp1");
        puts("EQS");
        puts("PUSHS GF@%tmp1");
        puts("ORS");
        break;
    case TOKEN_DIVISION:
        puts("DIVS");
        break;
    case TOKEN_FLOOR_DIVISION:
        puts("IDIVS");
        break;
    case TOKEN_MUL:
        puts("MULS");
        break;
    case TOKEN_SUB:
        puts("SUBS");
        break;
    case TOKEN_NOT_EQUAL_TO:
        puts("EQS");
        puts("NOTS");
        break;
    case TOKEN_STRING_CONCAT:
        puts("POPS GF@%tmp2");
        puts("POPS GF@%tmp1");
        puts("CONCAT GF@%tmp1 GF@%tmp1 GF@%tmp2");
        puts("PUSHS GF@%tmp1");
        break;
    case TOKEN_ADD:
        puts("ADDS");
        break;
    case TOKEN_STRING_LENGTH:
        puts("POPS GF@%tmp1");
        puts("STRLEN GF@%tmp1 GF@%tmp1");
        puts("PUSHS GF@%tmp1");
        break;
    default:
        /* Should not happen */
        ERR_MSG("Unexpected type of operator value: ", token->line);
        fprintf(stderr, "%d\n", token->type);
        break;
    }
}

void gen_expr_cond(void) { puts("POPS GF@%tmp1"); }

static void escape_string(dynamic_string_s **ds)
{
    /* TODO: implement */
    (void)ds;
}

void gen_write(tstack_s *in_params)
{
    T_token *token;

    while (!tstack_empty(in_params)) {
        token = tstack_top(in_params);
        tstack_pop(in_params, false);
        escape_string(&token->value);
        if (token->type == TOKEN_ID) {
            printf("WRITE LF@%s", token->value->content);
        } else {
            printf("WRITE string@%s", token->value->content);
        }
        puts("WRITE \010"); /* TODO: remove when escape_string is implemented */
    }
}

static void gen_reads(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

    gen_func_start("reads", &in_params, 1);
    printf("READ LF@%%retval1 string");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}

static void gen_readi(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

    gen_func_start("readi", &in_params, 1);
    printf("READ LF@%%retval1 int");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}
static void gen_readn(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

    gen_func_start("readn", &in_params, 1);
    printf("READ LF@%%retval1 float");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}
static void gen_tointeger(void)
{
    tstack_s in_params;
    T_token *p0;
    tstack_init(&in_params);
    p0 = malloc(sizeof *p0);
    ALLOC_CHECK(p0);
    tstack_push(&in_params, p0);

    gen_func_start("tointeger", &in_params, 1);
    printf("FLOAT2INT LF@%%retval1 LF@%%p0");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}
static void gen_substr(void)
{
    T_token *p0, *p1, *p2;
    tstack_s in_params;
    tstack_init(&in_params);
    p0 = malloc(sizeof *p0);
    ALLOC_CHECK(p0);
    tstack_push(&in_params, p0);
    p1 = malloc(sizeof *p1);
    ALLOC_CHECK(p1);
    tstack_push(&in_params, p1);
    p2 = malloc(sizeof *p2);
    ALLOC_CHECK(p2);
    tstack_push(&in_params, p2);

    gen_func_start("substr", &in_params, 1);
    puts("MOVE LF@%retval1 string@");
    puts("DEFVAR LF@%iter");
    puts("DEFVAR LF@%c");
    puts("DEFVAR LF@%end_index");
    puts("MOVE LF@%end_index LF@%p2");
    puts("MOVE LF@%iter LF@%p1");
    puts("SUB LF@%iter LF@%iter int@1");
    puts("DEFVAR LF@%cond");
    puts("DEFVAR LF@%strlen");
    puts("STRLEN LF@%strlen LF@%p0");
    puts("GT LF@%cond LF@%p1 LF@%p2");
    puts("JUMPIFEQ $-substr_end LF@%cond bool@true");
    puts("GT LF@%cond LF@%p1 int@0");
    puts("JUMPIFEQ $-substr_end LF@%cond bool@false");
    puts("GT LF@%cond LF@%p2 int@0");
    puts("JUMPIFEQ $-substr_end LF@%cond bool@false");
    puts("GT LF@%cond LF@%p2 LF@%strlen");
    puts("JUMPIFEQ $-substr_end LF@%cond bool@true");
    puts("LABEL $-strloop");
    puts("LT LF@%cond LF@%iter LF@%end_index");
    puts("JUMPIFEQ $-end_loop LF@%cond bool@false");
    puts("GETCHAR LF@%c LF@%p0 LF@%iter");
    puts("CONCAT LF@%retval1 LF@%retval1 LF@%c");
    puts("ADD LF@%iter LF@%iter int@1");
    puts("JUMP $-strloop");
    puts("LABEL $-end_loop");
    puts("LABEL $-substr_end");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}

static void gen_ord(void)
{
    T_token *p0, *p1;
    tstack_s in_params;
    tstack_init(&in_params);
    p0 = malloc(sizeof *p0);
    ALLOC_CHECK(p0);
    tstack_push(&in_params, p0);
    p1 = malloc(sizeof *p1);
    ALLOC_CHECK(p1);
    tstack_push(&in_params, p1);

    gen_func_start("ord", &in_params, 1);
    puts("DEFVAR LF@%index");
    puts("DEFVAR LF@%cond");
    puts("DEFVAR LF@%strlen");
    puts("STRLEN LF@%strlen LF@%p0");
    puts("GT LF@%cond LF@%index int@0");
    puts("JUMPIFEQ $-ord-err LF@%cond bool@false");
    puts("GT LF@%cond LF@%index LF@%strlen");
    puts("JUMPIFEQ $-ord-err LF@%cond bool@true");
    puts("JUMP $-ord-ok");
    puts("LABEL $-ord-err");
    puts("EXIT int@8");
    puts("LABEL $-ord-ok");
    puts("MOVE LF@%index LF@%p1");
    puts("SUB LF@%index LF@%index int@1");
    puts("STRI2INT LF@%retval1 LF@%p0 LF@%index");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}

static void gen_chr(void)
{
    T_token *p0;
    tstack_s in_params;
    tstack_init(&in_params);
    p0 = malloc(sizeof *p0);
    ALLOC_CHECK(p0);
    tstack_push(&in_params, p0);

    gen_func_start("chr", &in_params, 1);
    puts("DEFVAR LF@%cond");
    puts("EQ LF@%cond nil@nil LF@%p0");
    puts("JUMPIFEQ $-chr-ok LF@%cond bool@false");
    puts("EXIT int@8");
    puts("LABEL $-chr-ok");
    puts("LT LF@%cond LF@%p0 int@0");
    puts("JUMPIFEQ $-chr-end LF@%cond bool@true");
    puts("GT LF@%cond LF@%p0 int@255");
    puts("JUMPIFEQ $-chr-end LF@%cond bool@true");
    puts("INT2CHAR LF@%retval1 LF@%p0");
    puts("$-chr-end");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}

static void generate_built_ins(void)
{
    gen_reads();
    gen_readi();
    gen_readn();
    gen_tointeger();
    gen_substr();
    gen_ord();
    gen_chr();
}
