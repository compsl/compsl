#include "extern/vm.h"
#include "extern/compslerr.h"
#include "intern/vars.h"
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>

//TODO unbreak all the code broken by swiching to varTable instead of just sructs in the vm

/*TODO: add error checking and set compsl errorno, where possible, otherwise just
 * cleanup and return error condition
 */

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

float *vm_getFloat(VM *vm, const char *name)
{
	int i = findVar(&(vm->vt), name);
	if(i > 0)
    	return &(vm->vt.vars[i].v.f);
    	
    vm->errorno = COMPSL_NO_SUCH_VAR;
    return NULL;
}

int32_t *vm_getInt(VM *vm, const char *name)
{
	int i = findVar(&(vm->vt), name);
	if(i > 0)
		return &(vm->vt.vars[i].v.i);
	vm->errorno = COMPSL_NO_SUCH_VAR;
    return NULL;
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
	{
		vm->errorno = COMPSL_VARS_FULL;
		return NULL;
	}
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
	{
		vm->errorno = COMPSL_VARS_FULL;
		return NULL;
	}
}


bool addFunc(VM *vm, compsl_nat func, const char *name, const char *params, bool retFloat, bool isVoid)
{
	if(vm->ncnt == VM_NATIVEFN_INIT_SIZE)
	{
		vm->errorno = COMPSL_FUNC_FULL;
		return false;
	}
	
	char *tmp;
	
	vm->natives[vm->ncnt].func = func;
	
	//done with tmp so that name can be delcared const char
	tmp = malloc((strlen(name) + 1) * sizeof(char));
	strcpy(tmp,name);
	vm->natives[vm->ncnt].name = tmp;
	
	vm->natives[vm->ncnt].retFloat = retFloat;
	vm->natives[vm->ncnt].isVoid = isVoid;

	// parse params here.
	
	tmp = malloc((strlen(params) + 1)* sizeof(char));
	strcpy(tmp,params);
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

