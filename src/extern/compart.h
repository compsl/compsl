#ifndef COMPART_H_
#define COMPART_H_
#include "vm.h"
#include "var.h"
#include "compslerr.h"

#define COMPART_MAX_VARS 256
#define COMPART_MAX_CONSTS 256
#define COMPART_MAX_CUBBYS 16

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _COMPART_t
{
    varTable vt;
    //varTable ct;
    var cons[COMPART_MAX_CONSTS];
    uint16_t numConst;
    uint16_t numCubbys;
    
    struct _CUBBY_t
    {
    	void *code; // pointer to the bytecode of this cubby
    	char *name; // name of this cubby
    } cubbys[COMPART_MAX_CUBBYS];
    
    VM *vm; // the vm this compartment was compiled with
    
    COMPSL_ERROR errorno; // error code of last error produced by this compartment
} compart;

compart *createComp(VM *vm);
void destroyComp(compart *);

void com_addCubby(compart *, void *code, const char *name);
void com_prStats(compart *);

float *com_addFloat(compart *, const char *name);
int32_t *com_addInt(compart *, const char *name);

float *com_getFloat(compart *, const char *name);
int32_t *com_getInt(compart *, const char *name);

int16_t getCubbyID(compart *, const char *name);

#ifdef __cplusplus
}
#endif

#endif /*COMPART_H_*/
