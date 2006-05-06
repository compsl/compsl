#ifndef __COMPSL_COMPILER_H
#define __COMPSL_COMPILER_H

#include <stdio.h>
#include "compart.h"
#include "vm.h"
#include "var.h"
#include "compsl.h"


compart * fileCompiler(char *filename , VM*);
//compart * compiler(char *code, size_t , VM*);


#endif 