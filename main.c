/**
 * @file main.c
 * Projekt: Implementace prekladace imperativniho jazyka IFJ21.
 * @author Krystof Albrecht <xalbre05@stud.fit.vutbr.cz>
 * @brief Main file used for creating the executable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "scanner.h"

int main(void)
{
    initialize_scanner();
    return start_parsing();
}
