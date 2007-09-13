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

#include "extern/compsl.h"
#include "intern/compartment.h"
#include "intern/vars.h"
#include "compiler/node.h"

#include <malloc.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
 
COMPSL_EXPORT __attribute__ ((nonnull)) compart *createComp(VM *vm)
{
	compart *tmp = (compart *)malloc(sizeof(compart));
    if(tmp == NULL)
        return NULL;
	
	varTableCreate(&(tmp->vt), COMPART_MAX_VARS);
	//varTableCreate(&(tmp->ct), COMPART_MAX_CONSTS);
    tmp->numConst = 0;
    tmp->numCubbys = 0;
    
    for(int i = 0; i < COMPART_MAX_CUBBYS; i++)
    {
    	tmp->cubbys[i].code = NULL;
    	tmp->cubbys[i].name = NULL;
    }
    
    tmp->vm = vm;
    
    tmp->errorno = COMPSL_NOERR;
    
    return tmp;
}

COMPSL_EXPORT  __attribute__ ((nonnull)) void destroyComp(compart *c)
{
	varTableDestroy(&(c->vt));
	//varTableDestroy(&(c->ct));

	for(int i = 0; i < COMPART_MAX_CUBBYS && c->cubbys[i].code != NULL; i++)
    {
    	free(c->cubbys[i].code); c->cubbys[i].code = NULL;
    	free(c->cubbys[i].name); c->cubbys[i].name = NULL;
    }
    
	free(c);
}

COMPSL_EXPORT __attribute__ ((pure,nonnull)) int16_t getCubbyID(compart *com, const char *name)
{
	int i;
	
	for(i = 0; i < COMPART_MAX_CUBBYS && com->cubbys[i].code != NULL; i++)
	{
		if(!strcmp(name, com->cubbys[i].name))
			return i; 
	}
	
	com->errorno = COMPSL_NO_SUCH_CUBBY;
	return -1; // not found
}

COMPSL_EXPORT __attribute__ ((nonnull)) float *com_getFloat(compart *com, const char *name)
{
	int i = findVar(&(com->vt), name);
	if(i >= 0)
    	return &(com->vt.vars[findVar(&(com->vt), name)].v.f);
    com->errorno = COMPSL_NO_SUCH_VAR;
    return NULL;	
}

COMPSL_EXPORT  __attribute__ ((nonnull)) int32_t *com_getInt(compart *com, const char *name)
{
	int i = findVar(&(com->vt), name);
	if(i >= 0)
    	return &(com->vt.vars[findVar(&(com->vt), name)].v.i);
    com->errorno = COMPSL_NO_SUCH_VAR;
    return NULL;	
}

COMPSL_EXPORT  __attribute__ ((nonnull)) float *com_addFloat(compart *com, const char *name)
{
	var *tmp;
	if(com->vt.cnt < COMPART_MAX_VARS)
	{
		tmp = addVar(&(com->vt), FLOAT_VAR, name);
		return &(tmp->v.f);
	}
	else
	{
		com->errorno = COMPSL_VARS_FULL;
		return NULL;
	}
}

COMPSL_EXPORT  __attribute__ ((nonnull)) int32_t *com_addInt(compart *com, const char *name)
{
	var *tmp;
	if(com->vt.cnt < COMPART_MAX_VARS)
	{
		tmp = addVar(&(com->vt), INT_VAR, name);
		return &(tmp->v.i);
	}
	else
	{
		com->errorno = COMPSL_VARS_FULL;
		return NULL;
	}
}

COMPSL_INTERN int16_t  __attribute__ ((nonnull)) com_addConst(compart *com, intfloat val)
{
	if(com->numConst >= COMPART_MAX_CONSTS)
	{
		com->errorno = COMPSL_CONST_FULL;
		return -1;
	}
	for(int i = 0; i < com->numConst; i++)
	{
		if((com->cons[i].v.i) == val.i)
			return i;
	}
	
	com->cons[com->numConst].v = val;
	com->numConst ++;
	return com->numConst - 1;
}

COMPSL_INTERN int  __attribute__ ((nonnull)) com_addCubby(compart *com, void *code, const char *name) 
{
	if(com->numCubbys >= COMPART_MAX_CUBBYS)
	{
		com->errorno = COMPSL_CUBBYS_FULL;
		return false;
	}
	com->cubbys[com->numCubbys].code = code;
	com->cubbys[com->numCubbys].name = malloc((strlen(name)+1)*sizeof(char));
	if(com->cubbys[com->numCubbys].name == NULL)
	{
		com->errorno = COMPSL_NOMEM;
		return false;
	} 
	strcpy(com->cubbys[com->numCubbys].name, name);
	com->numCubbys++;
	return true;
}

COMPSL_INTERN int bc_len(bytecode *);

COMPSL_LOCAL void com_prStats(compart *com){
	for(int i=0;i<com->numCubbys;i++) {
		printf("(%s, %i) ",com->cubbys[i].name, bc_len(com->cubbys[i].code));
	}	
	
}
