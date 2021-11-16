/**
 * @file exp_parser.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for bottom-up parser for expressions.
 */

#ifndef _EXP_PARSER_H
#define _EXP_PARSER_H

#include "avl.h"
#include "scanner.h"
#include "symtable.h"

/**
 * @brief Parses expression. Creates Abstract Syntatic Tree.
 *
 * @param[in] symtable Table of symbols to search for identifiers.
 *
 * @return Syntactic tree. NULL on invalid expression syntax.
 */
avl_node_s *exp_parse(symtable_s *symtable);

#endif /* _EXP_PARSER_H */
