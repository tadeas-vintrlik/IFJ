/**
 * @file exp_parser.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for bottom-up parser for expressions.
 */

#ifndef _EXP_PARSER_H
#define _EXP_PARSER_H

#include "avl.h"
#include "common.h"
#include "scanner.h"
#include "symtable.h"

/**
 * @brief Parses expression. Calls code generator.
 *
 * @param[in] symtable Table of symbols to search for identifiers.
 *
 * @return If expression parsing succeeded.
 */
bool exp_parse(symtable_s *symtable);

#endif /* _EXP_PARSER_H */
