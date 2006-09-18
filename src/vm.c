#include "extern/vm.h"
#include "extern/compslerr.h"
#include "intern/vars.h"
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>

//TODO unbreak all the code broken by swiching to varTable instead of just sructs in the vm

/**
 * create a new VM
 */
VM *createVM(void)
{
    VM *tmp = (VM *)malloc(sizeof(VM));
    if(tmp == NULL)
        return NULL;
	
	varTableCreate(&(tmp->vt), VM_MAX_GVARS);
	
    tmp->natives = (nativeFN *)malloc(sizeof(nativeFN) * VM_NATIVEFN_INIT_SIZE);
    tmp->ncnt = 0;
    
    tmp->errorno = COMPSL_NOERR;
    
    return tmp;
}

/**
 * destroy a VM
 */
void destroyVM(VM *vm)
{
	int i;
    for(i = 0; i < vm->ncnt; i++)
		free(vm->natives[i].name);
	
	varTableDestroy(&(vm->vt));
	
	for(int i=0; i<vm->ncnt; i++)
	{
		if(vm->natives[i].numParam != 0)
		{
			free(vm->natives[i].params);
			free(vm->natives[i].paramFlags);
		}
	}
	
    free(vm->natives);
    free(vm);
}

float *vm_getFloat(VM *vm, const char *name)
{
    return &(vm->vt.vars[findVar(&(vm->vt), name)].v.f);	
}

int32_t *vm_getInt(VM *vm, const char *name)
{
	return &(vm->vt.vars[findVar(&(vm->vt), name)].v.i);
}

float *vm_addFloat(VM *vm, const char *name)
{
	var *tmp;
	if(vm->vt.cnt < VM_MAX_GVARS)
	{
		tmp = addVar(&(vm->vt), FLOAT_VAR, name);
		return &(tmp->v.f);
	}
	else
		return NULL;
}

int32_t *vm_addInt(VM *vm, const char *name)
{
	var *tmp;
	if(vm->vt.cnt < VM_MAX_GVARS)
	{
		tmp = addVar(&(vm->vt), INT_VAR, name);
		return &(tmp->v.i);
	}
	else
		return NULL;
}


bool addFunc(VM *vm, intfloat (*func)(var *), const char *name, const char *params, bool retFloat, bool isVoid)
{
	if(vm->ncnt == VM_NATIVEFN_INIT_SIZE)
	{
		//TODO: add setting errno here
		return false;
	}
	
	vm->natives[vm->ncnt].func = func;
	vm->natives[vm->ncnt].name = malloc((strlen(name) + 1) * sizeof(char));
	strcpy(name,vm->natives[vm->ncnt].name);
	vm->natives[vm->ncnt].retFloat = retFloat;
	vm->natives[vm->ncnt].isVoid = isVoid;

	// parse params here.
	
	char *tmp = malloc((strlen(params) + 1)* sizeof(char));
	strcpy(params,tmp);
	char * tok = strtok(tmp," ,");
	int tokCount = 0;
	while(tok != NULL)
	{
		tokCount++;
		tok= strtok(NULL," ,");
	}
	
	vm->natives[vm->ncnt].numParam = tokCount;
	vm->natives[vm->ncnt].paramFlags = malloc(tokCount * sizeof(uint8_t));
	vm->natives[vm->ncnt].params = malloc(tokCount * sizeof(var));
	
	strcpy(params,tmp);
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
			free(vm->natives[vm->ncnt].name);

			//set errorno here
			
			return false; // FAILURE!!!!!!111
		}
		tokCount++;
		tok= strtok(NULL," ,");
	}
		
	vm->ncnt++; // we are done adding another native!
	
	return true;
}

