/**
 * @file code_gen.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Implementation of code generator.
 */

#include "code_gen.h"
#include "common.h"
#include "sll.h"

static unsigned counter = 0;
static sll_s call_list;

typedef struct call {
    T_token *function;
    tstack_s *params_in;
} call_s;

static void generate_built_ins(void);

static call_s *call_constructor(T_token *function, tstack_s *params_in)
{
    call_s *result = malloc(sizeof *result);
    ALLOC_CHECK(result);
    result->function = function;
    result->params_in = params_in;
    return result;
}

static void call_destructor(call_s *call)
{
    /* call->function is stored on global frame and need not be freed here */
    tstack_destroy(call->params_in);
}

void gen_call_insert(T_token *function, tstack_s *params_in)
{
    sll_insert_last(&call_list, call_constructor(function, params_in));
}

void gen_prog_start(void)
{
    puts(".IFJcode21");
    puts("JUMP $%main");
    puts("DEFVAR GF@%tmp1");
    puts("DEFVAR GF@%tmp2");
    generate_built_ins();
    sll_init(&call_list);
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
    sll_activate(in_params);
    while (sll_is_active(in_params)) {
        token = sll_get_active(in_params);
        printf("DEFVAR LF@%s\n", token->value->content);
        printf("MOVE LF@%s LF@%%p%d\n", token->value->content, i);
        i++;
        sll_next(in_params);
    }
}

void gen_func_start(const char *func_name, tstack_s *in_params, unsigned no_returns)
{
    unsigned i = 0;

    /* Create label for calls to jump to and PUSH TF to LF */
    printf("\nLABEL $-%s\n", func_name);
    puts("PUSHFRAME");

    /* Create variables for return values */
    for (i = 0; i < no_returns; i++) {
        printf("DEFVAR LF@%%retval%d\n", i);
        printf("MOVE LF@%%retval%d nil@nil\n", i);
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
    while (sll_is_active(return_vals)) {
        token = sll_get_active(return_vals);
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
        sll_next(return_vals);
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

void gen_while_jump_loop(unsigned label_number) { printf("JUMP $-while%d\n", label_number); }

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
    if (!strcmp("write", func_name)) {
        gen_write(in_params);
    } else {
        puts("CREATEFRAME");
        gen_push_arg(in_params);
        printf("CALL $-%s\n", func_name);
    }
}

void gen_function_call_list(void)
{
    call_s *current_call;

    puts("LABEL $%main");
    puts("CREATEFRAME");
    puts("PUSHFRAME");
    sll_activate(&call_list);
    while (sll_is_active(&call_list)) {
        current_call = (call_s *)sll_get_active(&call_list);
        gen_func_call(current_call->function->value->content, current_call->params_in);
        call_destructor(current_call);
        sll_next(&call_list);
    }

    sll_destroy(&call_list, true);
}

static void escape_string(dynamic_string_s **ds)
{
    dynamic_string_s *new_ds = malloc(sizeof *new_ds);
    ALLOC_CHECK(new_ds);
    ds_init(new_ds);
    dynamic_string_s *old_ds = *ds;
    char c;
    for (unsigned i = 0; i < old_ds->size; i++) {
        switch (old_ds->content[i]) {
        case ' ':
            ds_add_char(new_ds, '\\');
            ds_add_char(new_ds, '0');
            ds_add_char(new_ds, '3');
            ds_add_char(new_ds, '2');
            break;
        case '\\':
            c = old_ds->content[++i];
            if (c) {
                switch (c) {
                case 'n':
                    ds_add_char(new_ds, '\\');
                    ds_add_char(new_ds, '0');
                    ds_add_char(new_ds, '1');
                    ds_add_char(new_ds, '0');
                    break;
                case 't':
                    ds_add_char(new_ds, '\\');
                    ds_add_char(new_ds, '0');
                    ds_add_char(new_ds, '0');
                    ds_add_char(new_ds, '9');
                    break;
                case '\"':
                    ds_add_char(new_ds, '\\');
                    ds_add_char(new_ds, '0');
                    ds_add_char(new_ds, '3');
                    ds_add_char(new_ds, '4');
                    break;
                case '\\':
                    ds_add_char(new_ds, '\\');
                    ds_add_char(new_ds, '0');
                    ds_add_char(new_ds, '9');
                    ds_add_char(new_ds, '2');
                    break;
                default:
                    ERR_MSG("Unexpected escape sequence in code_gen", -1);
                    break;
                }
            }
            break;
        default:
            ds_add_char(new_ds, old_ds->content[i]);
            break;
        }
    }
    ds_destroy(old_ds);
    FREE(old_ds);
    *ds = new_ds;
}

void gen_expr_operand(T_token *token)
{
    switch (token->type) {
    case TOKEN_ID:
        printf("PUSHS LF@%s\n", token->value->content);
        break;
    case TOKEN_INT:
        printf("PUSHS int@%s\n", token->value->content);
        break;
    case TOKEN_NUMBER:
        printf("PUSHS float@%s\n", token->value->content);
        break;
    case TOKEN_STRING:
        escape_string(&token->value);
        printf("PUSHS string@%s\n", token->value->content);
        break;
    case TOKEN_KEYWORD:
        if (!strcmp(token->value->content, "nil")) {
            puts("PUSHS nil@nil");
        } else {
            /* Should not happen */
            ERR_MSG("Unexpected type of operand value: keyword on line: ", token->line);
            fprintf(stderr, "%d\n", token->type);
        }
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

void gen_write(tstack_s *in_params)
{
    T_token *token;

    while (!tstack_empty(in_params)) {
        token = tstack_top(in_params);
        tstack_pop(in_params, false);
        escape_string(&token->value);
        if (token->type == TOKEN_ID) {
            printf("WRITE LF@%s\n", token->value->content);
        } else {
            printf("WRITE string@%s\n", token->value->content);
        }
    }
}

void gen_var_decl(T_token *id)
{
    printf("DEFVAR LF@%s\n", id->value->content);
    printf("MOVE LF@%s nil@nil\n", id->value->content);
}

static void gen_reads(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

    gen_func_start("reads", &in_params, 1);
    puts("READ LF@%retval0 string");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}

static void gen_readi(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

    gen_func_start("readi", &in_params, 1);
    puts("READ LF@%retval0 int");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}
static void gen_readn(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

    gen_func_start("readn", &in_params, 1);
    puts("READ LF@%retval0 float");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}
static void gen_tointeger(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

    gen_func_start("tointeger", &in_params, 1);
    puts("FLOAT2INT LF@%retval0 LF@%p0");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}
static void gen_substr(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

    gen_func_start("substr", &in_params, 1);
    puts("MOVE LF@%retval0 string@");
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
    puts("JUMPIFEQ $-substr_end LF@%cond bool@false");
    puts("GETCHAR LF@%c LF@%p0 LF@%iter");
    puts("CONCAT LF@%retval0 LF@%retval0 LF@%c");
    puts("ADD LF@%iter LF@%iter int@1");
    puts("JUMP $-strloop");
    puts("LABEL $-substr_end");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}

static void gen_ord(void)
{
    tstack_s in_params;
    tstack_init(&in_params);

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
    puts("STRI2INT LF@%retval0 LF@%p0 LF@%index");
    puts("POPFRAME");
    puts("RETURN");

    tstack_destroy(&in_params);
}

static void gen_chr(void)
{
    tstack_s in_params;

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
    puts("INT2CHAR LF@%retval0 LF@%p0");
    puts("LABEL $-chr-end");
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
