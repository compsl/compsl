#ifndef VARS_H_
#define VARS_H_

#include "../extern/var.h"
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../intern/vars.h"
#include <inttypes.h>
#include <stdbool.h>

#define INT_VAR 0
#define FLOAT_VAR 1
#define IS_ARRAY 2

/**
 * search for a variable in the specified list
 * returns index if found, -1 otherwise
 * list = list to search
 * name = name of var
 */
int16_t findVar(varTable *vt,const char *name) __attribute__ ((pure));

var *addVar(varTable *vt, uint8_t typeflags, const char *name);

void varTableCreate(varTable *vt, uint16_t size);
//free up the symbol table and variable values
// also free the names of the vars
void varTableDestroy(varTable *vt);

#endif /*VARS_H_*/
