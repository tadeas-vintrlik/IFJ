/**
 * @file common.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Common functions used in the entire project.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

void ds_init(dynamic_string_s *dynamic_string)
{
    dynamic_string->limit = 32;
    dynamic_string->line = calloc(dynamic_string->limit, sizeof(char));
    ALLOCK_CHECK(dynamic_string->line);
    dynamic_string->size = 0;
}

rc_e ds_add_char(dynamic_string_s *dynamic_string, const char c)
{
    /* Realloc line if not enough */
    if (dynamic_string->limit == dynamic_string->size) {
        dynamic_string->limit = 2 * dynamic_string->limit;
        dynamic_string->line = realloc(dynamic_string->line, dynamic_string->limit);
        ALLOCK_CHECK(dynamic_string->line);
    }

    /* Duplicate string and place the duplicate in the structure */

    dynamic_string->line[dynamic_string->size++] = c;
    return RC_OK;
}

void ds_destroy(dynamic_string_s *dynamic_string)
{
    FREE(dynamic_string->line);
    dynamic_string->limit = 0;
    dynamic_string->size = 0;
}

void sf_init(source_file_s *source)
{
    source->limit = 32;
    source->line = calloc(source->limit, sizeof source->line);
    ALLOCK_CHECK(source->line);
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
        ALLOCK_CHECK(source->line);
    }

    /* Duplicate string and place the duplicate in the structure */
    dup = strdup(line);
    ALLOCK_CHECK(dup);
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
