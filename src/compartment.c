#include "extern/vm.h"
#include "extern/compart.h"
#include "intern/vars.h"
#include "extern/var.h"
#include <malloc.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

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
void destroyComp(compart *c)
{
	varTableDestroy(&(c->vt));
	//varTableDestroy(&(c->ct));

	for(int i = 0; i < COMPART_MAX_CUBBYS && c->cubbys[i].code != NULL; i++)
    {
    	free(c->cubbys[i].code);
    	free(c->cubbys[i].name);
    }
    
    
	free(c);
}

int16_t getCubbyID(compart *com, const char *name)
{
	int i;
	
	for(i = 0; i < COMPART_MAX_CUBBYS && com->cubbys[i].code != NULL; i++)
	{
		if(!strcmp(name, com->cubbys[i].name))
			return i; 
	}
	
	return -1; // not found
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

void com_addCubby(compart *com, void *code, const char *name) {
	com->cubbys[com->numCubbys].code = code;
	com->cubbys[com->numCubbys].name = name;
	com->numCubbys++;
}

void com_prStats(compart *com){
	for(int i=0;i<com->numCubbys;i++) {
		printf("(%s, %i) ",com->cubbys[i].name, bc_len(com->cubbys[i].code))	;
	}	
	
}
