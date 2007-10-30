// $Id:vars.h 541 2007-10-01 01:19:09Z tomj $

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

#ifndef COMPSL_SYMS_H_
#define COMPSL_SYMS_H_

#include "compsl.h"
#include <stdbool.h>

#define INT_VAR 0
#define FLOAT_VAR 1
#define IS_ARRAY 2
#define IS_GLBL 4

struct SYMTABLE_T // don't bother with typedef, only here so I can sizeof it later
{
	uint16_t id;
	uint8_t typeflags;// OR together the apropriat flags ex float array = FLOAT_VAR | IS_ARRAY
	char *name; // symbol name
};

/**
 * Holds variables and their symbols, also tracks # of variables in it
 */
typedef struct VAR_TABLE_T
{
	uint16_t capacity; ///< max variables for this table
	uint16_t cnt; ///< number of variables currently in the table
    var *vars; ///< the variables themselves
    struct SYMTABLE_T *symbols; ///< the symbol table for the vars
} varTable;

/** 
 * Represents one paramenter to a native function.
 */
typedef struct native_param_t
{
	bool isFloat; ///< true if the param has type float, false if it's an int
	bool isArray; ///< if the param is an array true, false if it's scalar
} nativeParam;

/**
 * looks after one native function, holds pointer to function and
 * info about number and type of params
 */
typedef struct _nativeFN_t
{
	const char *name;   ///< name of this function
	compsl_nat func;    ///< pointer the function to call

	bool isVoid;        ///<true iff function does not return a value
	bool retFloat;      ///< true iff the return type is a float

	var *params;        ///< the list of paramaters to pass to it
	uint8_t *paramFlags;///< OR together the apropriat flags ex float array = FLOAT_VAR | IS_ARRAY
	uint8_t numParam;   ///< how many paramaters to pass it
} nativeFN;

typedef struct
{
	int16_t id;
	bool isvar;// if this is false then symbol is a native or built in function
	bool local;
	bool array;
	bool isfloat;
}symbolinfo; // used for symbol table search (for compiler)

/**
 * search all symbol tables and return info on a symbol
 * 
 * Post: if symbol doesn't exist returns a symbolinfo with id=-1
 * 		 else returns info on symbol
 */
COMPSL_INTERN COMPSL_PURE_NONNULL symbolinfo searchSym(const char *name, compart *com);

/**
 * search for a variable in the specified list
 * returns index if found, -1 otherwise
 * list = list to search
 * name = name of var
 */
COMPSL_INTERN COMPSL_PURE_NONNULL int16_t findVar(const varTable *vt,const char *name);

/**
 * add avariable to a varTable
 * returns pointer to the newly added variable
 * vt = the varTable to add to
 * typeflags = holds type info about the variable
 * name = the name of the variable
 */
COMPSL_INTERN COMPSL_NONNULL var *addVar(varTable *vt, uint8_t typeflags, const char *name);


COMPSL_INTERN COMPSL_NONNULL void varTableCreate(varTable *vt, uint16_t size);

/**
 * free up the symbol table and variable values also free the names of the vars
 * vt = the varTable to free
 */
COMPSL_INTERN COMPSL_NONNULL void varTableDestroy(varTable *vt);

COMPSL_INTERN bool var_testEqi(var* a, var* b);
COMPSL_INTERN bool var_testEqf(var* a, var* b);

#endif /*VARS_H_*/
