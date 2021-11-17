/**
 * @file scanner.c
 * @author Josef Škorpík <xskorp07@stud.fit.vutbr.cz>
 * @author Kryštof Albrecht <xalbre05@stud.fit.vutbr.cz>
 * @brief Scanner implementation.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

typedef enum {
    STATE_START, // starting state
    STATE_ID_OR_KEYWORD,
    STATE_INT,
    STATE_DOT,
    STATE_NUMBER,
    STATE_EXP_NUMBER,
    STATE_EXPONENT,
    STATE_STRING_START,
    STATE_STRING_ESCAPE,
    STATE_STRING_ESCAPE_FIRST_ZERO_ONE,
    STATE_STRING_ESCAPE_FIRST_TWO,
    STATE_STRING_ESCAPE_SECOND_ZERO_TO_FOUR,
    STATE_STRING_ESCAPE_SECOND_ZERO_TO_NINE,
    STATE_STRING_ESCAPE_SECOND_FIVE,
    STATE_EQUAL_OR_DECLAR,
    STATE_LESS_THAN_OR_LESS_EQUAL,
    STATE_GREATER_THAN_OR_GREATER_EQUAL,
    STATE_NOT_EQUAL,
    STATE_CONCAT,
    STATE_SUB_OR_COMMENT,
    STATE_LINE_OR_BLOCK_COMMENT,
    STATE_LINE_OR_BLOCK_COMMENT_2,
    STATE_LINE_COMMENT,
    STATE_BLOCK_COMMENT,
    STATE_BLOCK_COMMENT_ENDING,
    STATE_DIV_OR_FLOOR_DIV,

} State;
static bool is_keyword(char *word)
{
    char *keywords[] = { "do", "else", "end", "function", "global", "if", "integer", "local", "nil",
        "number", "require", "return", "string", "then", "while" };

    for (int i = 0; i < 15; i++) {
        if (!strcmp(word, keywords[i])) {
            return true;
        }
    }

    return false;
}

T_token *held_token;

void unget_token(T_token *token)
{
    if (held_token == NULL) {
        held_token = token;
    } else {
        exit(RC_INTERNAL_ERR);
    }
}

void token_destroy(T_token *token)
{
    if (token == NULL) {
        return;
    }
    ds_destroy(token->value);
    free(token->value);
    free(token);
}

T_token *get_next_token()
{
    if (held_token != NULL) {
        T_token *result = held_token;
        held_token = NULL;

        return result;
    }

    static int curr_line = 1;
    // initializce dynamic string str

    T_token *token = malloc(sizeof(T_token));
    ALLOC_CHECK(token);
    token->value = malloc(sizeof(dynamic_string_s));
    ALLOC_CHECK(token->value);

    ds_init(token->value);

    token->line = curr_line;
    dynamic_string_s *str = token->value;

    // first, state is at the start
    State state = STATE_START;

    char c;
    while (1) {
        c = getc(stdin);
        // switch statement for FSM
        switch (state) {
        case (STATE_START):
            if (isalpha(c) || c == '_') {
                ds_add_char(str, c);
                state = STATE_ID_OR_KEYWORD;
            }
            // with whitespaces we go back to start
            else if (c == '\n' || c == '\t' || c == '\r' || c == ' ' || c == EOF) {
                if (c == '\n') {
                    curr_line++;
                    token->line = curr_line;
                } else if (c == EOF) {
                    token->type = TOKEN_EOF;
                    return token;
                }
                state = STATE_START;
            } else if (isdigit(c)) {
                ds_add_char(str, c);
                state = STATE_INT;
            } else if (c == '"') {
                state = STATE_STRING_START;
            } else if (c == '+') {
                token->type = TOKEN_ADD;
                return token;
            } else if (c == '*') {
                token->type = TOKEN_MUL;
                return token;
            } else if (c == '#') {
                token->type = TOKEN_STRING_LENGTH;
                return token;
            } else if (c == '(') {
                token->type = TOKEN_LEFT_BRACKET;
                return token;
            } else if (c == ')') {
                token->type = TOKEN_RIGHT_BRACKET;
                return token;
            } else if (c == ',') {
                token->type = TOKEN_COMMA;
                return token;
            } else if (c == ':') {
                token->type = TOKEN_COLON;
                return token;
            } else if (c == '=') {
                state = STATE_EQUAL_OR_DECLAR;
            } else if (c == '<') {
                state = STATE_LESS_THAN_OR_LESS_EQUAL;
            } else if (c == '>') {
                state = STATE_GREATER_THAN_OR_GREATER_EQUAL;
            } else if (c == '~') {
                state = STATE_NOT_EQUAL;
            } else if (c == '-') {
                state = STATE_SUB_OR_COMMENT;
            } else if (c == '.') {
                state = STATE_CONCAT;
            } else if (c == '/') {
                state = STATE_DIV_OR_FLOOR_DIV;
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_ID_OR_KEYWORD:
            if (isalpha(c) || isdigit(c) || c == '_') {
                ds_add_char(str, c);
            } else {
                if (is_keyword(str->content)) {
                    token->type = TOKEN_KEYWORD;
                } else {
                    token->type = TOKEN_ID;
                }

                ungetc(c, stdin);

                return token;
            }

            break;
        case STATE_INT:
            if (isdigit(c)) {
                state = STATE_INT;
                ds_add_char(str, c);
            } else if (c == 'e' || c == 'E') {
                state = STATE_EXPONENT;
                ds_add_char(str, c);
            } else if (c == '.') {
                state = STATE_DOT;
                ds_add_char(str, c);
            } else {
                token->type = TOKEN_INT;
                ungetc(c, stdin);

                return token;
            }

            break;
        case STATE_DOT:
            if (isdigit(c)) {
                state = STATE_NUMBER;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_EXPONENT:
            if (isdigit(c) || c == '+' || c == '-') {
                state = STATE_EXP_NUMBER;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_NUMBER:
            if (isdigit(c)) {
                state = STATE_NUMBER;
                ds_add_char(str, c);
            } else if (c == 'e' || c == 'E') {
                state = STATE_EXPONENT;
                ds_add_char(str, c);
            } else {
                token->type = TOKEN_NUMBER;
                ungetc(c, stdin);

                return token;
            }

            break;
        case STATE_EXP_NUMBER:
            if (isdigit(c)) {
                state = STATE_EXP_NUMBER;
                ds_add_char(str, c);
            } else {
                token->type = TOKEN_NUMBER;
                ungetc(c, stdin);

                return token;
            }

            break;
        case STATE_STRING_START:
            if (c == '\n') {
                exit(RC_LEX_ERR);
            } else if (c == '\\') {
                state = STATE_STRING_ESCAPE;
                ds_add_char(str, c);
            } else if (c == '"') {
                token->type = TOKEN_STRING;
                return token;
            } else if (c >= 31) {
                state = STATE_STRING_START;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_STRING_ESCAPE:
            if (c == '0' || c == '1') {
                state = STATE_STRING_ESCAPE_FIRST_ZERO_ONE;
                ds_add_char(str, c);
            } else if (c == '2') {
                state = STATE_STRING_ESCAPE_FIRST_TWO;
                ds_add_char(str, c);
            } else if (c == 'n' || c == 't' || c == '"' || c == '\\') {
                state = STATE_STRING_START;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_STRING_ESCAPE_FIRST_TWO:
            if (c == '5') {
                state = STATE_STRING_ESCAPE_SECOND_FIVE;
                ds_add_char(str, c);
            } else if (c >= '0' && c <= '4') {
                state = STATE_STRING_ESCAPE_SECOND_ZERO_TO_FOUR;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_STRING_ESCAPE_FIRST_ZERO_ONE:
            if (isdigit(c)) {
                state = STATE_STRING_ESCAPE_SECOND_ZERO_TO_NINE;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_STRING_ESCAPE_SECOND_FIVE:
            if (c >= '0' && c <= '5') {
                state = STATE_STRING_START;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;

        case STATE_STRING_ESCAPE_SECOND_ZERO_TO_FOUR:
            if (isdigit(c)) {
                state = STATE_STRING_START;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_STRING_ESCAPE_SECOND_ZERO_TO_NINE:
            if (isdigit(c)) {
                state = STATE_STRING_START;
                ds_add_char(str, c);
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_EQUAL_OR_DECLAR:
            if (c == '=') {
                token->type = TOKEN_EQUAL;
            } else {
                token->type = TOKEN_DECLAR;
                ungetc(c, stdin);
            }

            return token;
        case STATE_LESS_THAN_OR_LESS_EQUAL:
            if (c == '=') {
                token->type = TOKEN_LESS_EQUAL_THAN;
            } else {
                token->type = TOKEN_LESS_THAN;
                ungetc(c, stdin);
            }

            return token;
        case STATE_GREATER_THAN_OR_GREATER_EQUAL:
            if (c == '=') {
                token->type = TOKEN_GREATER_EQUAL_THAN;
            } else {
                token->type = TOKEN_GREATER_THAN;
                ungetc(c, stdin);
            }

            return token;
        case STATE_NOT_EQUAL:
            if (c == '=') {
                token->type = TOKEN_NOT_EQUAL_TO;
                return token;
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_DIV_OR_FLOOR_DIV:
            if (c == '/') {
                token->type = TOKEN_FLOOR_DIVISION;
            } else {
                token->type = TOKEN_DIVISION;
                ungetc(c, stdin);
            }

            return token;
        case STATE_CONCAT:
            if (c == '.') {
                token->type = TOKEN_STRING_CONCAT;
            } else {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_SUB_OR_COMMENT:
            if (c == '-') {
                state = STATE_LINE_OR_BLOCK_COMMENT;
            } else {
                token->type = TOKEN_SUB;
                ungetc(c, stdin);

                return token;
            }

            break;
        case STATE_LINE_OR_BLOCK_COMMENT:
            if (c == '[') {
                state = STATE_LINE_OR_BLOCK_COMMENT_2;
            } else {
                ungetc(c, stdin);
                state = STATE_LINE_COMMENT;
            }

            break;
        case STATE_LINE_OR_BLOCK_COMMENT_2:
            if (c == '[') {
                state = STATE_BLOCK_COMMENT;
            } else {
                state = STATE_LINE_COMMENT;
            }

            break;
        case STATE_LINE_COMMENT:
            if (c == '\n') {
                state = STATE_START;
                ungetc(c, stdin);
            }

            break;
        case STATE_BLOCK_COMMENT:
            if (c == ']') {
                state = STATE_BLOCK_COMMENT_ENDING;
            } else if (c == EOF) {
                exit(RC_LEX_ERR);
            }

            break;
        case STATE_BLOCK_COMMENT_ENDING:
            if (c == ']') {
                state = STATE_START;
            } else {
                state = STATE_BLOCK_COMMENT;
            }

            break;
        }
    }

    return token;
}
