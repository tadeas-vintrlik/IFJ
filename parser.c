#include "parser.h"

#define GET_CHECK(TYPE)                                                                            \
    token = get_next_token();                                                                      \
    if (token->type != TYPE)                                                                       \
        return false;

#define GET_CHECK_CMP(TYPE, VALUE)                                                                 \
    token = get_next_token();                                                                      \
    if (token->type != TYPE || strcmp(VALUE, token->value->content) != 0)                          \
        return false;

static bool rule_PROG();
static bool rule_CODE();
static bool rule_TOP_ELEM();

static bool rule_DECL();
static bool rule_DEF();
static bool rule_CALL();

static bool rule_PARAM_LIST();
static bool rule_NEXT_PARAM();
static bool rule_PARAM();

static bool rule_RET_LIST();
static bool rule_TYPE_LIST();
static bool rule_NEXT_TYPE();
static bool rule_TYPE();

static bool rule_ARG_LIST();
static bool rule_ARG();
static bool rule_NEXT_ARG();

static bool rule_BODY();

static bool rule_IF_ELSE();
static bool rule_WHILE();
static bool rule_EXPR(); // TODO Use the expression analyzer
static bool rule_VAR_DECL();

static bool magic_function();
static bool right_side_function(sll_s *left_side_ids);

bool start_parsing() { return rule_PROG(); }

static bool rule_PROG()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_KEYWORD && !strcmp("require", token->value->content)) {
        free(token);
        token = get_next_token();

        if (token->type == TOKEN_STRING && !strcmp("ifj21", token->value->content)) {
            return rule_CODE();
        }
    }

    return false;
}

static bool rule_CODE()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_EOF) {
        free(token);
        return true;
    } else {
        unget_token(token);
        return rule_TOP_ELEM() && rule_CODE();
    }
}

static bool rule_TOP_ELEM()
{
    T_token *first_token = get_next_token();

    if (first_token->type == TOKEN_ID) {
        // TODO Semantic
        unget_token(first_token);
        return rule_CALL();
    } else if (first_token->type == TOKEN_KEYWORD) {
        unget_token(first_token);
        if (!strcmp("global", first_token->value->content)) {
            return rule_DECL();
        } else if (!strcmp("function", first_token->value->content)) {
            return rule_DEF();
        }
    }

    return false;
}

static bool rule_CALL()
{
    T_token *token;
    GET_CHECK(TOKEN_ID); // TODO Semantic
    free(token);

    GET_CHECK(TOKEN_LEFT_BRACKET);
    free(token);

    if (!rule_ARG_LIST()) {
        return false;
    }

    GET_CHECK(TOKEN_RIGHT_BRACKET);
    free(token);

    return true;
}

static bool rule_DECL()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "global");
    free(token);

    GET_CHECK(TOKEN_ID); // TODO Semantic
    free(token);

    GET_CHECK(TOKEN_COLON);
    free(token);
    GET_CHECK_CMP(TOKEN_KEYWORD, "function");
    free(token);
    GET_CHECK(TOKEN_LEFT_BRACKET);
    free(token);

    if (!rule_TYPE_LIST()) {
        return false;
    }

    GET_CHECK(TOKEN_RIGHT_BRACKET);
    free(token);

    return rule_RET_LIST();
}

static bool rule_DEF()
{
    T_token *token;
    GET_CHECK_CMP(TOKEN_KEYWORD, "function");
    free(token);

    GET_CHECK(TOKEN_ID);
    free(token);

    GET_CHECK(TOKEN_LEFT_BRACKET);
    free(token);

    if (!rule_PARAM_LIST()) {
        return false;
    }

    GET_CHECK(TOKEN_RIGHT_BRACKET);
    free(token);

    if (!rule_RET_LIST() || !rule_BODY()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    free(token);

    return true;
}

static bool rule_PARAM_LIST()
{
    if (!rule_PARAM()) {
        return true;
    }

    return rule_NEXT_PARAM();
}

static bool rule_NEXT_PARAM()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COMMA) {
        free(token);
        return rule_PARAM() && rule_NEXT_PARAM();
    } else {
        unget_token(token);
        return true;
    }
}

static bool rule_PARAM()
{
    T_token *token = get_next_token();

    if (token->type != TOKEN_ID) {
        unget_token(token);
        return false;
    }

    GET_CHECK(TOKEN_COLON);
    free(token);

    return rule_TYPE();
}

static bool rule_RET_LIST()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COLON) {
        free(token);
        return rule_TYPE() && rule_NEXT_TYPE();
    } else {
        unget_token(token);
        return true;
    }
}

static bool rule_TYPE_LIST()
{
    if (!rule_TYPE()) {
        return true;
    }

    return rule_NEXT_TYPE();
}

static bool rule_NEXT_TYPE()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COMMA) {
        free(token);
        return rule_TYPE() && rule_NEXT_TYPE();
    } else {
        unget_token(token);
        return true;
    }
}

static bool rule_TYPE()
{
    // TODO Semantic
    T_token *token = get_next_token();

    bool is_type = token->type == TOKEN_KEYWORD
        && (!strcmp("nil", token->value->content) || !strcmp("integer", token->value->content)
            || !strcmp("number", token->value->content)
            || !strcmp("string", token->value->content));

    if (!is_type) {
        unget_token(token);
    } else {
        free(token);
    }

    return is_type;
}

static bool rule_ARG_LIST()
{
    if (!rule_ARG()) {
        return true;
    }

    return rule_NEXT_ARG();
}

static bool rule_ARG()
{
    T_token *token = get_next_token();

    switch (token->type) {
    case TOKEN_ID: // TODO Semantic
    case TOKEN_NUMBER:
    case TOKEN_INT:
    case TOKEN_STRING:
        return true;
        break;
    case TOKEN_KEYWORD:
        if (!strcmp("nil", token->value->content)) {
            return true;
        }

        unget_token(token);
        return false;
    default:
        unget_token(token);
        return false;
    }
}

static bool rule_NEXT_ARG()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COMMA) {
        free(token);
        return rule_ARG() && rule_NEXT_ARG();
    } else {
        unget_token(token);
        return true;
    }
}

///////////

static bool rule_BODY()
{
    T_token *token = get_next_token();

    switch (token->type) {
    case TOKEN_KEYWORD:
        unget_token(token);

        if (!strcmp("return", token->value->content)) {
            token = get_next_token();
            free(token);

            return rule_ARG_LIST();
        } else if (!strcmp("if", token->value->content)) {
            return rule_IF_ELSE() && rule_BODY();
        } else if (!strcmp("while", token->value->content)) {
            return rule_WHILE() && rule_BODY();
        } else if (!strcmp("local", token->value->content)) {
            return rule_VAR_DECL() && rule_BODY();
        }

        return true;
    case TOKEN_ID:
        unget_token(token);

        return magic_function() && rule_BODY();
    default:
        unget_token(token);
        return true;
    }
}

static bool rule_IF_ELSE()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "if");
    free(token);

    if (!rule_EXPR()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "then");
    free(token);

    if (!rule_BODY()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "else");
    free(token);

    if (!rule_BODY()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    free(token);

    return true;
}

static bool rule_WHILE()
{
    T_token *token;

    GET_CHECK_CMP(TOKEN_KEYWORD, "while");
    free(token);

    if (!rule_EXPR()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "do");
    free(token);

    if (!rule_BODY()) {
        return false;
    }

    GET_CHECK_CMP(TOKEN_KEYWORD, "end");
    free(token);

    return true;
}

static bool rule_VAR_DECL()
{
    T_token *token;
    sll_s id;
    sll_init(&id);

    GET_CHECK_CMP(TOKEN_KEYWORD, "local");
    free(token);

    // TODO Semantika
    GET_CHECK(TOKEN_ID);
    sll_insert_head(&id, token);
    free(token);

    GET_CHECK(TOKEN_COLON);
    free(token);

    if (!rule_TYPE()) {
        return false;
    }

    // TODO Insert symbol into symtable

    GET_CHECK(TOKEN_DECLAR);
    free(token);

    // TODO Error on multiple expressions on right side
    return right_side_function(&id);
}

static bool rule_EXPR() { return true; } // TODO Use the expression analyzer

static bool magic_function()
{
    T_token *token = get_next_token();
    sll_s left_side_ids;
    sll_init(&left_side_ids);

    if (token->type != TOKEN_ID) {
        return false;
    }

    T_token *token2 = get_next_token();

    unget_token(token2);
    unget_token(token);

    if (token2->type == TOKEN_LEFT_BRACKET) {
        return right_side_function(&left_side_ids);
    }

    while (true) {
        token = get_next_token();

        if (token->type != TOKEN_ID) {
            return false;
        }

        // TODO Change to sll_insert_last, right_side_function reads from left to right
        sll_insert_head(&left_side_ids, token);

        token = get_next_token();

        if (token->type == TOKEN_DECLAR) {
            break;
        }

        if (token->type != TOKEN_COMMA) {
            return false;
        }
    }

    return right_side_function(&left_side_ids);
}

static bool right_side_function(sll_s *left_side_ids)
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_ID) {
        T_token *token2 = get_next_token();
        if (token2->type == TOKEN_LEFT_BRACKET) {
            unget_token(token2);
            unget_token(token);

            return rule_CALL();
        }
        unget_token(token2);
    }

    unget_token(token);

    while (true) {
        token = get_next_token();

        if (!rule_EXPR()) {
            return false;
        }

        // TODO Assignment!

        sll_delete_head(left_side_ids, false);

        token = get_next_token();

        if (token->type != TOKEN_COMMA) {
            unget_token(token);
            break;
        }
    }

    return sll_is_empty(left_side_ids);
}
