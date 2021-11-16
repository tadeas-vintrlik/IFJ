/**
 * @file exp_parser.c
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header for bottom-up parser for expressions.
 */

#ifndef _EXP_PARSER_H
#define _EXP_PARSER_H

#include "avl.h"

/**
 * @brief Parses expression. Creates Abstract Syntatic Tree.
 *
 * @return Syntactic tree. NULL on invalid expression syntax.
 */
avl_node_s *exp_parse(void);

#endif /* _EXP_PARSER_H */
