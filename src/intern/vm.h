/***********************************************
 * INTERNAL VM HEADER
 ***********************************************/

#ifndef VMINT_H_
#define VMINT_H_
#include "../extern/vm.h"
#include "../intern/vars.h"
#include <inttypes.h>
#include <stdbool.h>

int16_t vm_findVar(VM *vm, const char *name) __attribute__ ((pure));// I tthink this should probably be removed...

/** returns true if the vm's variable table is full (max variables reached)
 */
bool vm_varFull(VM *vm) __attribute__ ((pure)); // do we actually need this?

#endif /*VMINT_H_*/
