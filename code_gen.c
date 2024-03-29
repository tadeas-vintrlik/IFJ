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
                    ds_add_char(new_ds, '\\');
                    ds_add_char(new_ds, old_ds->content[i]);
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

static void escape_float(dynamic_string_s **ds)
{
    dynamic_string_s *new = malloc(sizeof(dynamic_string_s));
    ALLOC_CHECK(new);
    ds_init(new);

    float raw_value = atof((*ds)->content);

    int buffer_size = 32;
    char *str = malloc(buffer_size);
    ALLOC_CHECK(str);

    snprintf(str, buffer_size, "%a", raw_value);

    char *current_char = str;
    while (*current_char != '\0') {
        ds_add_char(new, *current_char);
        current_char++;
    }

    FREE(str);

    ds_destroy(*ds);
    free(*ds);

    *ds = new;
}

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
    puts("DEFVAR GF@%tmp1");
    puts("DEFVAR GF@%tmp2");
    puts("JUMP $%main");
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

void gen_func_end()
{
    /* Push current LF as TF for caller to pop the return values and return to addres */
    puts("POPFRAME");
    puts("RETURN");
}

unsigned gen_jump_else(void)
{
    unsigned ret;
    puts("POPS GF@%tmp1");
    printf("JUMPIFNEQ $-else-%d GF@%%tmp1 bool@true\n", counter);
    ret = counter;
    counter++;
    return ret;
}

void gen_else_label(unsigned label_number)
{
    printf("JUMP $-end-%d\n", label_number);
    printf("LABEL $-else-%d\n", label_number);
}

void gen_if_end(unsigned label_number) { printf("LABEL $-end-%d\n", label_number); }

unsigned gen_while_label(void)
{
    unsigned ret;
    printf("LABEL $-while-%d\n", counter);
    ret = counter;
    counter++;
    return ret;
}

void gen_while_jump_loop(unsigned label_number) { printf("JUMP $-while-%d\n", label_number); }

void gen_while_jump_end(unsigned label_number)
{
    puts("POPS GF@%tmp1");
    printf("JUMPIFNEQ $-end-%d GF@%%tmp1 bool@true\n", label_number);
}

void gen_while_end_label(unsigned label_number) { printf("LABEL $-end-%d\n", label_number); }

static void gen_push_arg(T_token *fun_symbol, tstack_s *in_params)
{
    T_token *token;
    unsigned i = 0;

    sll_activate(fun_symbol->fun_info->in_params);

    /* Move all in paramters values to correct variables */
    while (!tstack_empty(in_params)) {
        T_token *fun_in = sll_get_active(fun_symbol->fun_info->in_params);
        token = tstack_top(in_params);
        tstack_pop(in_params, false);
        printf("DEFVAR TF@%%p%d\n", i);
        switch (token->type) {
        case TOKEN_ID:
            printf("MOVE TF@%%p%d LF@%s\n", i, token->value->content);
            break;
        case TOKEN_INT:
            printf("MOVE TF@%%p%d int@%s\n", i, token->value->content);
            if (fun_in->symbol_type == SYM_TYPE_NUMBER) {
                printf("INT2FLOAT TF@%%p%d TF@%%p%d\n", i, i);
            }
            break;
        case TOKEN_NUMBER:
            escape_float(&token->value);
            printf("MOVE TF@%%p%d float@%s\n", i, token->value->content);
            break;
        case TOKEN_STRING:
            printf("MOVE TF@%%p%d string@%s\n", i, token->value->content);
            break;
        case TOKEN_KEYWORD:
            if (!strcmp(token->value->content, "nil")) {
                printf("MOVE TF@%%p%d nil@nil\n", i);
            } else {
                /* Should not happen */
                ERR_MSG("Unexpected type of input parameter: keyword on line: ", token->line);
                fprintf(stderr, "%d\n", token->type);
            }
            break;
        default:
            /* Should not happen */
            ERR_MSG("Unexpected type of input parameter: ", token->line);
            fprintf(stderr, "%d\n", token->type);
            break;
        }

        i++;
        sll_next(fun_symbol->fun_info->in_params);
        FREE(token);
    }
}

void gen_func_call(T_token *fun_symbol, tstack_s *in_params)
{
    if (!strcmp("write", fun_symbol->value->content)) {
        gen_write(in_params);
    } else {
        puts("CREATEFRAME");
        gen_push_arg(fun_symbol, in_params);
        printf("CALL $-%s\n", fun_symbol->value->content);
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
        gen_func_call(current_call->function, current_call->params_in);
        call_destructor(current_call);
        sll_next(&call_list);
    }

    sll_destroy(&call_list, true);
    puts("EXIT int@0");
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
        escape_float(&token->value);
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
        puts("CALL $-check-div-error");
        puts("DIVS");
        break;
    case TOKEN_FLOOR_DIVISION:
        puts("CALL $-check-div-error");
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
        puts("CREATEFRAME");
        puts("DEFVAR TF@%p0");
        if (token->type == TOKEN_ID) {
            printf("MOVE TF@%%p0 LF@%s\n", token->value->content);
        } else if (token->symbol_type == SYM_TYPE_NUMBER) {
            escape_float(&token->value);
            printf("MOVE TF@%%p0 float@%s\n", token->value->content);
        } else {
            printf("MOVE TF@%%p0 string@%s\n", token->value->content);
        }
        puts("CALL $-write");
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

    puts("TYPE GF@%tmp1 LF@%p0");
    puts("JUMPIFNEQ $-tointeger-ok GF@%tmp1 string@nil");
    puts("MOVE LF@%retval0 nil@nil");
    puts("POPFRAME");
    puts("RETURN");
    puts("LABEL $-tointeger-ok");
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
    puts("MOVE LF@%index LF@%p1");
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

static void gen_builtin_write(void)
{
    tstack_s in_params;

    gen_func_start("write", &in_params, 1);
    puts("EQ GF@%tmp1 LF@%p0 nil@nil");
    puts("JUMPIFNEQ $-non-nil GF@%tmp1 bool@true");
    puts("WRITE string@nil");
    puts("JUMP $-write-end");
    puts("LABEL $-non-nil");
    puts("WRITE LF@%p0");
    puts("LABEL $-write-end");
    puts("POPFRAME");
    puts("RETURN");
}

static void gen_check_div_error(void)
{
    puts("LABEL $-check-div-error");
    puts("POPS GF@%tmp1");
    puts("TYPE GF@%tmp2 GF@%tmp1");
    puts("JUMPIFEQ $-div-integer GF@%tmp2 string@int");
    puts("JUMPIFEQ $-div-err GF@%tmp1 float@0x0p+0");
    puts("JUMP $-div-ok");
    puts("LABEL $-div-integer");
    puts("JUMPIFEQ $-div-err GF@%tmp1 int@0");
    puts("LABEL $-div-ok");
    puts("PUSHS GF@%tmp1");
    puts("RETURN");
    puts("LABEL $-div-err");
    puts("WRITE string@Zero\\032Division\\032Error!\\010");
    puts("EXIT int@9");
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
    gen_builtin_write();
    gen_check_div_error();
}
