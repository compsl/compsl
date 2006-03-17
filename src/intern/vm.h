/***********************************************
 * INTERNAL VM HEADER
 ***********************************************/

#ifndef VMINT_H_
#define VMINT_H_
#include "../extern/vm.h"
#include "../intern/vars.h"
#include <inttypes.h>
#include <stdbool.h>

int16_t vm_findVar(VM *vm, const char *name);

/** returns true if the vm's variable table is full (max variables reached)
 */
bool vm_varFull(VM *vm); 

#endif /*VMINT_H_*/
