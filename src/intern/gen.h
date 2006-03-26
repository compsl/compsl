#ifndef GEN_H_
#define GEN_H_
#include "../extern/var.h"
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../intern/vars.h"
#include <inttypes.h>
#include <stdbool.h>

typedef struct
{
	int16_t id;
	bool isvar;// if this is false then symbol is a native or built in function
	bool local;
	bool constant;
	bool array;
}symbolinfo; // used for symbol table search (for compiler)

/**
 * search all symbol tables and return info on a symbol
 */
symbolinfo searchSym(const char *name, compart *com) __attribute__ ((pure, fastcall));

#endif /*GEN_H_*/
