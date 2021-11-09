#include "parser.h"
#include <stdbool.h>

static bool rule_PROG();
static bool rule_CODE();
static bool rule_TOP_ELEM();

static bool rule_DECL();
static bool rule_DEF();
static bool rule_CALL();

static bool rule_ARG_LIST();
static bool rule_ARG();
static bool rule_NEXT_ARG();

bool start_parsing() { return rule_PROG(); }

static bool rule_PROG()
{
    T_token token;
    get_next_token(&token);

    if (token.type == TOKEN_KEYWORD && !strcmp("require", token.value->content)) {
        get_next_token(&token);

        if (token.type == TOKEN_STRING && !strcmp("ifj21", token.value->content)) {
            return rule_CODE();
        }
    }

    return false;
}

static bool rule_CODE()
{
    T_token token;
    get_next_token(&token);

    if (token.type == TOKEN_EOF) {
        return true;
    } else {
        unget_token(&token);
        return rule_TOP_ELEM();
    }
}

static bool rule_TOP_ELEM()
{
    T_token token;
    get_next_token(&token);

    if (token.type == TOKEN_ID) {
        unget_token(&token);
        return rule_CALL();
    } else if (token.type == TOKEN_KEYWORD) {
        unget_token(&token);
        if (!strcmp("global", token.value->content)) {
            return rule_DECL();
        } else if (!strcmp("function", token.value->content)) {
            return rule_DEF();
        }
    }

    return false;
}

static bool rule_CALL()
{
    T_token token;
    get_next_token(&token);

    if (token.type != TOKEN_ID) {
        return false; // TODO Semantika
    }

    get_next_token(&token);

    if (token.type != TOKEN_LEFT_BRACKET) {
        return false;
    }

    return rule_ARG_LIST();
}

static bool rule_ARG_LIST()
{
    T_token token;
    get_next_token(&token);

    if (token.type == TOKEN_RIGHT_BRACKET) {
        return true;
    } else {
        unget_token(&token);
        return rule_ARG() && rule_NEXT_ARG();
    }
}

static bool rule_ARG()
{
    T_token token;
    get_next_token(&token);

    switch (token.type) {
    case TOKEN_ID: // TODO Semantika
    case TOKEN_NUMBER:
    case TOKEN_INT:
    case TOKEN_STRING:
        return true;
        break;
    case TOKEN_KEYWORD:
        if (!strcmp("nil", token.value->content)) {
            return true;
        }
    default:
        return false;
    }
}

static bool rule_NEXT_ARG()
{
    T_token token;
    get_next_token(&token);

    if (token.type != TOKEN_COMMA) {
        return false;
    } else if (token.type == TOKEN_RIGHT_BRACKET) {
        return true;
    }

    return rule_ARG() && rule_NEXT_ARG();
}
