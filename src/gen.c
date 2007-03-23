#include "intern/gen.h"
#include "extern/var.h"
#include "intern/vars.h"
#include "intern/panic.h"
//#include "intern/builtins.h"
#include <inttypes.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

__attribute__ ((pure)) symbolinfo searchSym(const char *name, compart *com) 
{
	symbolinfo res;
	
	res.id = findVar(&(com->vt), name);
	
	if(res.id != -1)
	{
		res.isvar = res.local = true;
		res.array = com->vt.symbols[res.id].typeflags & IS_ARRAY;
		res.isfloat = com->vt.symbols[res.id].typeflags & FLOAT_VAR;
		return res;
	}
	
	
	res.id = findVar(&(com->vm->vt), name);
	
	if(res.id != -1)
	{
		res.isvar = true;
		res.local = false;
		res.array = com->vm->vt.symbols[res.id].typeflags & IS_ARRAY;
		res.isfloat = com->vm->vt.symbols[res.id].typeflags & FLOAT_VAR;
		return res;
	}
	
	res.array = res.isvar = res.local = false;
	res.id = -1;
	
	for(int i = 0; i < com->vm->ncnt; i++)
	{
		if(strcmp(name, com->vm->natives[i].name) == 0)
		{
			res.id=i;
			res.isvar = false;
			res.local = false;
			res.array = false;
			res.isfloat = com->vm->natives[i].retFloat;
			break;
		}
	}
	
	return res;
}

void panic(const char *s)
{
	//TODO: writeme
	puts(s);
	
	//TODO: what to do, we want this to be a fatal contidion for the lib
	// but we don't want to make the caller die...
}
