#include "extern/vm.h"
#include "extern/compart.h"
#include "intern/vars.h"
#include "extern/var.h"
#include <malloc.h>
#include <stdlib.h>
#include <inttypes.h>

/*
 * TODO: 
 *  1. write insert variable functions
 *  2. write insert constant functions
 *  3. write other stuff
 */
 
compart *createComp(VM *vm)
{
	compart *tmp = (compart *)malloc(sizeof(compart));
    if(tmp == NULL)
        return NULL;
	
	varTableCreate(&(tmp->vt), COMPART_MAX_VARS);
	//varTableCreate(&(tmp->ct), COMPART_MAX_CONSTS);
    tmp->numConst = 0;
    
    tmp->vm = vm;
    
    tmp->errorno = COMPSL_NOERR;
    
    return tmp;
}
void destroyComp(compart *c)
{
	varTableDestroy(&(c->vt));
	//varTableDestroy(&(c->ct));
	
	free(c);
}

float *com_getFloat(compart *com, const char *name)
{
    return &(com->vt.vars[findVar(&(com->vt), name)].v.f);	
}

int32_t *com_getInt(compart *com, const char *name)
{
	return &(com->vt.vars[findVar(&(com->vt), name)].v.i);
}

float *com_addFloat(compart *com, const char *name)
{
	var *tmp;
	if(com->vt.cnt < VM_MAX_GVARS)
	{
		tmp = addVar(&(com->vt), FLOAT_VAR, name);
		return &(tmp->v.f);
	}
	else
		return NULL;
}

int32_t *com_addInt(compart *com, const char *name)
{
	var *tmp;
	if(com->vt.cnt < VM_MAX_GVARS)
	{
		tmp = addVar(&(com->vt), INT_VAR, name);
		return &(tmp->v.i);
	}
	else
		return NULL;
}

int16_t com_addConst(compart *com, intfloat val)
{
	for(int i = 0; i < com->numConst; i++)
	{
		if((com->cons[i].v.i) == val.i)
			return i;
	}
	
	com->cons[com->numConst].v = val;
	com->numConst ++;
}
