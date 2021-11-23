/**
 * @file common.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @brief Common functions used in the entire project.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

char *my_strdup(const char *str)
{
    int len = strlen(str);
    char *new = malloc((len + 1) * sizeof *new);
    if (!new) {
        return NULL;
    }
    strcpy(new, str);
    new[len + 1] = '\0';
    return new;
}

void ds_init(dynamic_string_s *dynamic_string)
{
    dynamic_string->limit = 32;
    dynamic_string->content = calloc(dynamic_string->limit, sizeof(char));
    ALLOC_CHECK(dynamic_string->content);
    dynamic_string->size = 0;
}

rc_e ds_add_char(dynamic_string_s *dynamic_string, const char c)
{
    /* Realloc line if not enough */
    if (dynamic_string->limit == dynamic_string->size) {
        dynamic_string->limit = 2 * dynamic_string->limit;
        dynamic_string->content = realloc(dynamic_string->content, dynamic_string->limit);
        ALLOC_CHECK(dynamic_string->content);
    }

    /* Duplicate string and place the duplicate in the structure */

    dynamic_string->content[dynamic_string->size++] = c;

    // Add null byte at the end
    if (c != '\0') {
        ds_add_char(dynamic_string, '\0');
        dynamic_string->size--;
    }

    return RC_OK;
}

void ds_destroy(dynamic_string_s *dynamic_string)
{
    if (!dynamic_string) {
        return;
    }
    FREE(dynamic_string->content);
    dynamic_string->limit = 0;
    dynamic_string->size = 0;
}

void sf_init(source_file_s *source)
{
    source->limit = 32;
    source->line = calloc(source->limit, sizeof source->line);
    ALLOC_CHECK(source->line);
    source->no_lines = 0;
}

rc_e sf_add_line(source_file_s *source, const char *line)
{
    char *dup;

    /* Check paramaters */
    if (!line) {
        return RC_INTERNAL_ERR;
    }

    /* Realloc line if not enough */
    if (source->limit == source->no_lines) {
        source->limit = 2 * source->limit;
        source->line = realloc(source->line, source->limit);
        ALLOC_CHECK(source->line);
    }

    /* Duplicate string and place the duplicate in the structure */
    dup = my_strdup(line);
    ALLOC_CHECK(dup);
    source->line[source->no_lines++] = dup;
    return RC_OK;
}

void sf_destroy(source_file_s *source)
{
    for (unsigned i = 0; i < source->no_lines; i++) {
        FREE(source->line[i]);
    }
    FREE(source->line);
    source->limit = 0;
    source->no_lines = 0;
}
void printer(int value)
{
    switch (value) {
    case 0:
        printf("ID ");
        return;
    case 1:
        printf("KEYWORD ");
        return;
    case 2:
        printf("INT ");
        return;
    case 3:
        printf("NUMBER ");
        return;
    case 4:
        printf("STRING ");
        return;
    case 5:
        printf("EQUAL ");
        return;
    case 6:
        printf("DECLAR ");
        return;
    case 7:
        printf("LESS_THAN ");
        return;
    case 8:
        printf("LESS_EQUAL_THAN ");
        return;
    case 9:
        printf("GREATER_THAN ");
        return;
    case 10:
        printf("GREATER_EQUAL_THAN ");
        return;
    case 11:
        printf("DIVISION ");
        return;
    case 12:
        printf("FLOOR_DIVISION ");
        return;
    case 13:
        printf("MUL ");
        return;
    case 14:
        printf("SUB ");
        return;
    case 15:
        printf("NOT_EQUAL_TO ");
        return;
    case 16:
        printf("ADD ");
        return;
    case 17:
        printf("STRING_LENGTH ");
        return;
    case 18:
        printf("LEFT_BRACKET ");
        return;
    case 19:
        printf("RIGHT_BRACKET ");
        return;
    case 20:
        printf("COMMA ");
        return;
    case 21:
        printf("COLON ");
        return;
    case 22:
        printf("EOF ");
        return;
    }
}

void err_token_printer(unsigned type)
{
    switch (type) {
    case 5:
        fprintf(stderr, "=");
        break;
    case 6:
        fprintf(stderr, ":=");
        break;
    case 7:
        fprintf(stderr, "<");
        break;
    case 8:
        fprintf(stderr, "<=");
        break;
    case 9:
        fprintf(stderr, ">");
        break;
    case 10:
        fprintf(stderr, ">=");
        break;
    case 11:
        fprintf(stderr, "/");
        break;
    case 12:
        fprintf(stderr, "//");
        break;
    case 13:
        fprintf(stderr, "*");
        break;
    case 14:
        fprintf(stderr, "-");
        break;
    case 15:
        fprintf(stderr, "~=");
        break;
    case 16:
        fprintf(stderr, "..");
        break;
    case 17:
        fprintf(stderr, "+");
        break;
    case 18:
        fprintf(stderr, "#");
        break;
    case 19:
        fprintf(stderr, "(");
        break;
    case 20:
        fprintf(stderr, ")");
        break;
    case 21:
        fprintf(stderr, ",");
        break;
    case 22:
        fprintf(stderr, ":");
        break;
    case 23:
        fprintf(stderr, "end of file");
        break;
    default:
        break;
    }
}
