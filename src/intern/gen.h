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

#ifndef GEN_H_
#define GEN_H_
#include "../extern/compsl.h"
#include "../intern/vars.h"
#include <stdbool.h>

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
COMPSL_INTERN symbolinfo searchSym(const char *name, compart *com) __attribute__ ((pure));

#ifdef DEBUG
void dumpBytecode(compart *com, int id);
#endif

#endif /*GEN_H_*/
