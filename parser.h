/**
 * @file parser.h
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Krystof Albrecht <xalbre05@stud.fit.vutbr.cz>
 * @author Josef Skorpik <xskorp07@stud.fit.vutbr.cz>
 * @author Jakub Kozubek <xkozub07@stud.fit.vutbr.cz>
 * @author Tadeas Vintrlik <xvintr04@stud.fit.vutbr.cz>
 * @brief Header file for the parser module.
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "code_gen.h"
#include "exp_parser.h"
#include "scanner.h"
#include "sll.h"
#include "symtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

rc_e start_parsing();

#endif
