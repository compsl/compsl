#ifndef GEN_H_
#define GEN_H_
#include "../extern/compsl.h"
#include "../intern/vars.h"
#include <stdbool.h>

typedef struct
{
	int16_t id;
	bool isvar;// if this is false then symbol is a native or built in function
	bool local;
	bool array;
	bool isfloat;
}symbolinfo; // used for symbol table search (for compiler)

/**
 * search all symbol tables and return info on a symbol
 * 
 * Post: if symbol doesn't exist returns a symbolinfo with id=-1
 * 		 else returns info on symbol
 */
symbolinfo searchSym(const char *name, compart *com) __attribute__ ((pure));

#ifdef DEBUG
void dumpBytecode(compart *com, int id);
#endif

#endif /*GEN_H_*/
