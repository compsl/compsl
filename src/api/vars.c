// $Id:vars.c 543 2007-10-01 01:31:02Z tomj $

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
#include "intern/gen.h"
#include "intern/syms.h"
#include <inttypes.h>
#include <malloc.h>
#include <string.h>
#include <float.h>
#include <math.h>

void varTableCreate(varTable *vt, uint16_t size)//FIXME uh what did I put this here for
{
	vt->cnt = 0;
	vt->capacity = size;
	vt->symbols = malloc(sizeof(struct SYMTABLE_T) * size);
	vt->vars = malloc(sizeof(var) * size);
}

void varTableDestroy(varTable *vt)
{
	int i;
	for(i = 0; i < vt->cnt; i++)
	{
		free(vt->symbols[i].name);// free the name string
		if(vt->symbols[i].typeflags & IS_ARRAY)
		{
			free(vt->vars[i].p);// free the array
			vt->vars[i].p = NULL;
		}
	}
	
	free(vt->symbols); vt->symbols = NULL;
	free(vt->vars); vt->vars = NULL;
}

__attribute__ ((pure)) int16_t findVar(const varTable *vt, const char *name)
{
	for(int i = 0; i < vt->cnt; i++)
	{
		if(strcmp(vt->symbols[i].name, name) == 0)// is this the one?
			return i;
	}
	
	return -1; //didn't find it
}

var *addVar(varTable *vt, uint8_t typeflags, const char *name)
{	
	if(vt->cnt >= vt->capacity) return NULL;
	
	int16_t tmp = findVar(vt, name);
	
	if(tmp == -1)
	{
		vt->vars[vt->cnt].size = -1;
		vt->vars[vt->cnt].v.i = 0;
			
		vt->symbols[vt->cnt].id = vt->cnt;
		vt->symbols[vt->cnt].name = malloc(sizeof(char)*(strlen(name) + 1));
		if(vt->symbols[vt->cnt].name == NULL) return NULL;
		strcpy(vt->symbols[vt->cnt].name, name);
		vt->symbols[vt->cnt].typeflags = typeflags;
			
		return &(vt->vars[vt->cnt++]);
	}
	else if(typeflags == vt->symbols[tmp].typeflags)
	{
		return &(vt->vars[tmp]);
	}

	return NULL;
}

bool var_testEqi(var* a, var* b) {
  if(a->size!=b->size) return false;

  if(a->size>0) {
    for(int i=0;i<a->size;i++)
      if(a->p[i].i != b->p[i].i) 
	return false;
    return true;

  } else 
    return (a->v.i==b->v.i);
}
bool var_testEqf(var* a, var* b) {
  if(a->size!=b->size) return false;

  if(a->size>0) {
    for(int i=0;i<a->size;i++) {
      if(fabs(a->p[i].f - b->p[i].f)>= FLT_EPSILON) 
	return false;
    }
    return true;
  } else 
    return (fabs(a->v.f-b->v.f) < FLT_EPSILON);
  
}
