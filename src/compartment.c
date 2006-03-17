#include "intern/vm.h"
#include "extern/vm.h"
#include "extern/compart.h"
#include <malloc.h>
#include <stdlib.h>
#include <inttypes.h>

/*
 * TODO: 
 *  1. write insert variable functions
 *  2. write insert constant functions
 *  3. write other stuff
 */
 
compart *createComp(void)
{
	compart *tmp = (compart *)malloc(sizeof(compart));
    if(tmp == NULL)
        return NULL;
	
	varTableCreate(&(tmp->vt), COMPART_MAX_VARS);
	varTableCreate(&(tmp->ct), COMPART_MAX_CONSTS);
    
    tmp->errno = COMPSL_NOERR;
    
    return tmp;
}
void destroyComp(compart *c)
{
	
}
