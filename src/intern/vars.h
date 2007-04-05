// $Id$

/*
    CompSL scripting language 
    Copyright (C) 2007  Thomas Jones & John Peberdy

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef VARS_H_
#define VARS_H_

#include "../extern/compsl.h"
#include <stdbool.h>

#define INT_VAR 0
#define FLOAT_VAR 1
#define IS_ARRAY 2
#define IS_GLBL 4

/**
 * search for a variable in the specified list
 * returns index if found, -1 otherwise
 * list = list to search
 * name = name of var
 */
COMPSL_INTERN int16_t findVar(const varTable *vt,const char *name) __attribute__ ((pure));

COMPSL_INTERN var *addVar(varTable *vt, uint8_t typeflags, const char *name);

COMPSL_INTERN void varTableCreate(varTable *vt, uint16_t size);
//free up the symbol table and variable values
// also free the names of the vars
COMPSL_INTERN void varTableDestroy(varTable *vt);

COMPSL_INTERN bool var_testEqi(var* a, var* b);
COMPSL_INTERN bool var_testEqf(var* a, var* b);

#endif /*VARS_H_*/
