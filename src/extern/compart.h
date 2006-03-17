#ifndef COMPART_H_
#define COMPART_H_
#include "vm.h"
#include "var.h"
#include "compslerr.h"

#define COMPART_MAX_VARS 256
#define COMPART_MAX_CONSTS 256
#define COMPART_MAX_CUBBYS 16

typedef struct _COMPART_t
{
    varTable vt;
    varTable ct;
    
    struct
    {
    	void *code; // pointer to the bytecode of this cubby
    	char *name; // name of this cubby
    } cubbys[COMPART_MAX_CUBBYS];
    
    VM *vm; // the vm this compartment was compiled with
    
    COMPSL_ERROR errno; // error code of last error produced by this compartment
} compart;

compart *createComp(void);
void destroyComp(compart *);



#endif /*COMPART_H_*/
