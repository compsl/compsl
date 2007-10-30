// $Id:gen.c 543 2007-10-01 01:31:02Z tomj $

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

#include "intern/gen.h"
#include "intern/syms.h"
#include "intern/panic.h"
#include "intern/compartment.h"
#include "compsl.h"
#include <inttypes.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <time.h>


COMPSL_INTERN COMPSL_PURE_NONNULL symbolinfo searchSym(const char *name, compart *com) 
{
	symbolinfo res;
	
	res.id = findVar(&(com->vt), name);
	
	if(res.id != -1)
	{
		res.isvar = res.local = true;
		res.array = com->vt.symbols[res.id].typeflags & IS_ARRAY;
		res.isfloat = com->vt.symbols[res.id].typeflags & FLOAT_VAR;
		return res;
	}
	
	
	res.id = findVar(&(com->vm->vt), name);
	
	if(res.id != -1)
	{
		res.isvar = true;
		res.local = false;
		res.array = com->vm->vt.symbols[res.id].typeflags & IS_ARRAY;
		res.isfloat = com->vm->vt.symbols[res.id].typeflags & FLOAT_VAR;
		return res;
	}
	
	res.array = res.isvar = res.local = false;
	res.id = -1;
	res.isfloat = false;
	
	for(int i = 0; i < com->vm->ncnt; i++)
	{
		if(strcmp(name, com->vm->natives[i].name) == 0)
		{
			res.id=i;
			res.isvar = false;
			res.local = false;
			res.array = false;
			res.isfloat = com->vm->natives[i].retFloat;
			break;
		}
	}
	
	return res;
}



__attribute__ ((constructor)) void init_func(void)
{
	
}

void panic(const char *s)
{
	//TODO: writeme
	puts(s);
	
	//TODO: what to do, we want this to be a fatal contidion for the lib
	// but we don't want to make the caller die...
}

