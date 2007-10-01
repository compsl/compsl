// $Id:vm.c 543 2007-10-01 01:31:02Z tomj $

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
#include "intern/vm.h"
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

/*TODO: add error checking and set compsl errorno, where possible, otherwise just
 * cleanup and return error condition
 */
void init_genrand(unsigned long s);
/**
 * create a new VM
 */
COMPSL_EXPORT VM *createVM(void)
{
    VM *tmp = (VM *)malloc(sizeof(VM));
    if(tmp == NULL)
        return NULL;
	
	varTableCreate(&(tmp->vt), VM_MAX_GVARS);
	
    tmp->natives = (nativeFN *)malloc(sizeof(nativeFN) * VM_NATIVEFN_INIT_SIZE);
    tmp->ncnt = 0;
    
    tmp->errorno = COMPSL_NOERR;
    
    //seed the rng
	init_genrand(time(0));
    
    return tmp;
}

/**
 * destroy a VM
 */
COMPSL_EXPORT void destroyVM(VM *vm)
{	
	varTableDestroy(&(vm->vt));
	
	for(int i=0; i<vm->ncnt; i++)
	{
		if(vm->natives[i].numParam != 0)
		{
			free(vm->natives[i].params); vm->natives[i].params = NULL; 
			free(vm->natives[i].paramFlags); vm->natives[i].paramFlags = NULL;
		}
		free((void *)(vm->natives[i].name)); 
		vm->natives[i].name = (const char *)NULL;
	}
	
    free(vm->natives); vm->natives = NULL; 
    free(vm);
}

COMPSL_EXPORT float *vm_getFloat(VM *vm, const char *name)
{
	int i = findVar(&(vm->vt), name);
	if(i >= 0)
    	return &(vm->vt.vars[i].v.f);
    	
    vm->errorno = COMPSL_NO_SUCH_VAR;
    return NULL;
}

COMPSL_EXPORT int32_t *vm_getInt(VM *vm, const char *name)
{
	int i = findVar(&(vm->vt), name);
	if(i >= 0)
		return &(vm->vt.vars[i].v.i);
	vm->errorno = COMPSL_NO_SUCH_VAR;
    return NULL;
}

COMPSL_EXPORT float *vm_addFloat(VM *vm, const char *name)
{
	var *tmp;
	if(vm->vt.cnt < VM_MAX_GVARS)
	{
		tmp = addVar(&(vm->vt), FLOAT_VAR, name);
		return &(tmp->v.f);
	}
	else
	{
		vm->errorno = COMPSL_VARS_FULL;
		return NULL;
	}
}

COMPSL_EXPORT int32_t *vm_addInt(VM *vm, const char *name)
{
	var *tmp;
	if(vm->vt.cnt < VM_MAX_GVARS)
	{
		tmp = addVar(&(vm->vt), INT_VAR, name);
		return &(tmp->v.i);
	}
	else
	{
		vm->errorno = COMPSL_VARS_FULL;
		return NULL;
	}
}


COMPSL_EXPORT bool addFunc(VM *vm, compsl_nat func, const char *name, const char *params, bool retFloat, bool isVoid)
{
	if(vm->ncnt == VM_NATIVEFN_INIT_SIZE)
	{
		vm->errorno = COMPSL_FUNC_FULL;
		return false;
	}
	
	if(vm == NULL || name == NULL || params == NULL)
	{
		vm->errorno = COMPSL_NULL_POINTER;
		return false;
	}
	
	vm->natives[vm->ncnt].func = func;
	
	//done with tmp so that name can be delcared const char
	char *tmp = malloc((strlen(name) + 1) * sizeof(char));
	if(tmp == NULL)
	{
		vm->errorno = COMPSL_NOMEM;
		return false;
	}
	strcpy(tmp,name);
	vm->natives[vm->ncnt].name = tmp;
	
	vm->natives[vm->ncnt].retFloat = retFloat;
	vm->natives[vm->ncnt].isVoid = isVoid;

	// parse params here.
	
	tmp = malloc((strlen(params) + 1)* sizeof(char));
	if(tmp == NULL)
	{
		free((void *)(vm->natives[vm->ncnt].name));
		vm->errorno = COMPSL_NOMEM;
		return false;
	}
	strcpy(tmp,params);
	char * tok = strtok(tmp," ,");
	int tokCount = 0;
	while(tok != NULL)
	{
		tokCount++;
		tok= strtok(NULL," ,");
	}
	
	if(tokCount == 0)
	{
		vm->natives[vm->ncnt].numParam = 0;
		vm->natives[vm->ncnt].paramFlags = NULL;
		vm->natives[vm->ncnt].params = NULL;
		free(tmp);
		vm->ncnt++; // we are done adding another native!
	
		return true;
		
	}
	vm->natives[vm->ncnt].numParam = tokCount;
	vm->natives[vm->ncnt].paramFlags = malloc(tokCount * sizeof(uint8_t));
	vm->natives[vm->ncnt].params = malloc(tokCount * sizeof(var));
	
	strcpy(tmp,params);
	tok = strtok(tmp," ,");
	tokCount=0;
	while(tok != NULL)
	{ // does this work?
		//figure out what the type of the param is
		if(strcmp(tok,"int") == 0)
		{
			vm->natives[vm->ncnt].params[tokCount].size = -1;
			vm->natives[vm->ncnt].paramFlags[tokCount] = 0;
		}
		else if(strcmp(tok,"float") == 0)
		{
			vm->natives[vm->ncnt].params[tokCount].size = -1;
			vm->natives[vm->ncnt].paramFlags[tokCount] = FLOAT_VAR;
		}
		else if(strcmp(tok,"int[]") == 0)
		{
			vm->natives[vm->ncnt].params[tokCount].size = -1;
			vm->natives[vm->ncnt].paramFlags[tokCount] = IS_ARRAY;
		}
		else if(strcmp(tok,"float[]") == 0)
		{
			vm->natives[vm->ncnt].params[tokCount].size = -1;
			vm->natives[vm->ncnt].paramFlags[tokCount] = FLOAT_VAR | IS_ARRAY;
		}
		else
		{
			free(vm->natives[vm->ncnt].params);
			free(vm->natives[vm->ncnt].paramFlags);
			free((void *)(vm->natives[vm->ncnt].name));
			
			free(tmp);
			vm->natives[vm->ncnt].name = NULL;
			vm->natives[vm->ncnt].params = NULL;
			vm->natives[vm->ncnt].paramFlags = NULL;
			
			vm->errorno = COMPSL_BAD_PARM_FRMT;
			return false; // FAILURE!!!!!!111
		}
		tokCount++;
		tok= strtok(NULL," ,");
	}
	
	free(tmp);
		
	vm->ncnt++; // we are done adding another native!
	
	return true;
}

