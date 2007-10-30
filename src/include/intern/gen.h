// $Id:gen.h 541 2007-10-01 01:19:09Z tomj $

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

#ifndef COMPSL_GEN_H_
#define COMPSL_GEN_H_
#include "compsl.h"
#include "bytecode.h"
#include <stdbool.h>

#ifdef DEBUG
COMPSL_INTERN COMPSL_PURE_NONNULL void dumpBytecode(compart *com, int id);
COMPSL_INTERN COMPSL_PURE_NONNULL void dumpBytecode2(compart *com, const bytecode *pc);
#else
#define dumpBytecode(...) 
#define dumpBytecode2(...)
#endif

#endif /*GEN_H_*/
