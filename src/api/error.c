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

#include "compsl.h"
#include <stdio.h>

static const char *emessgs[] =
{//TODO: fill in errors
	"No Error",
	"Out of Memory",
	"VARIABLE TABLE FULL: Cannot add variable",
	"CONSTANT TABLE FULL: Cannot add constant",
	"Cubbyhole Table full: Too many cubbyholes",
	"Function Table full: Too many functions",
	"No Such Cubbyhole: the specified name does not exist",
	"No Such Variable: the specified name does not exist",
	"No Such Function: the specified name does not exist",
	"Cannot add function: Bad Function Parameter Format",
	"Unexpected NULL pointer",
	"Cannot modify: Compartment already compiled",
	"Undefined Error!",
	NULL
};

//static char *noSuchMessage = "Non-existant error message";

COMPSL_EXPORT const char *compsl_getErrstr(COMPSL_ERROR err)
{
	err=(err > COMPSL_UNDEF_ERR && err >= 0)?COMPSL_UNDEF_ERR:err; // bounds check
	return emessgs[err];
}

COMPSL_EXPORT void compsl_printErr(COMPSL_ERROR err)
{
	err=(err > COMPSL_UNDEF_ERR && err >= 0) ? COMPSL_UNDEF_ERR: err; // bounds check
	fprintf(stderr, "ERROR %i: %s\n", err, emessgs[err]);
}

