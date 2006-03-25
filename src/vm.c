#include "extern/vm.h"
#include "extern/compslerr.h"
#include "intern/vm.h"
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

/*************************************************************
 * Internal functions
 *************************************************************/
int16_t vm_findVar(VM *vm, const char *name)
{
	return findVar(&(vm->vt), name);
}
