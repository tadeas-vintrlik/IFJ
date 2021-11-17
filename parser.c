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
static bool rule_TYPE(bool should_unget);

static bool rule_ARG_LIST();
static bool rule_ARG();
static bool rule_NEXT_ARG();

static bool rule_BODY();

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

    GET_CHECK(TOKEN_LEFT_BRACKET);

    return rule_ARG_LIST();
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
    T_token *token;

    GET_CHECK(TOKEN_ID);
    free(token);

    GET_CHECK(TOKEN_COLON);
    free(token);

    return rule_TYPE(false);
}

static bool rule_RET_LIST()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COLON) {
        free(token);
        return rule_TYPE(false) && rule_NEXT_TYPE();
    } else {
        unget_token(token);
        return true;
    }
}

static bool rule_TYPE_LIST()
{
    if (!rule_TYPE(true)) {
        return true;
    }

    return rule_NEXT_TYPE();
}

static bool rule_NEXT_TYPE()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_COMMA) {
        free(token);
        return rule_TYPE(false) && rule_NEXT_TYPE();
    } else {
        unget_token(token);
        return true;
    }
}

static bool rule_TYPE(bool should_unget_on_fail)
{
    // TODO Semantic
    T_token *token = get_next_token();

    bool is_type = token->type == TOKEN_KEYWORD
        && (!strcmp("nil", token->value->content) || !strcmp("integer", token->value->content)
            || !strcmp("number", token->value->content)
            || !strcmp("string", token->value->content));

    if (should_unget_on_fail && !is_type) {
        unget_token(token);
    } else {
        free(token);
    }

    return is_type;
}

static bool rule_ARG_LIST()
{
    T_token *token = get_next_token();

    if (token->type == TOKEN_RIGHT_BRACKET) {
        return true;
    } else {
        unget_token(token);
        return rule_ARG() && rule_NEXT_ARG();
    }
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

        return false;
    default:
        return false;
    }
}

static bool rule_NEXT_ARG()
{
    T_token *token = get_next_token();

    switch (token->type) {
    case TOKEN_COMMA:
        return rule_ARG() && rule_NEXT_ARG();
    case TOKEN_RIGHT_BRACKET:
        return true;
    default:
        return false;
    }
}

///////////

static bool rule_BODY()
{
    /* ____ ____  _____   ____   ___  ______   __ */
    /* / ___|  _ \| ____| | __ ) / _ \|  _ \ \ / / */
    /* | |  _| | | |  _|   |  _ \| | | | | | \ V / */
    /* | |_| | |_| | |___  | |_) | |_| | |_| || | */
    /* \____|____/|_____| |____/ \___/|____/ |_| */

    return true;
}
