#include "intern/gen.h"
#include "extern/var.h"
#include "intern/vars.h"
#include "intern/panic.h"
#include <inttypes.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

__attribute__ ((pure, fastcall)) symbolinfo searchSym(const char *name, compart *com) 
{
	symbolinfo res;
	
	res.id = findVar(&(com->ct), name);
	
	if(res.id != -1)
	{
		res.isvar = res.local = res.constant = true;
		res.array = com->ct.symbols[res.id].typeflags & IS_ARRAY;
		res.isfloat = com->ct.symbols[res.id].typeflags & FLOAT_VAR;
		return res;
	}
	
	res.id = findVar(&(com->vt), name);
	
	if(res.id != -1)
	{
		res.isvar = res.local = true;
		res.constant = false;
		res.array = com->vt.symbols[res.id].typeflags & IS_ARRAY;
		res.isfloat = com->vt.symbols[res.id].typeflags & FLOAT_VAR;
		return res;
	}
	
	
	res.id = findVar(&(com->vm->vt), name);
	
	if(res.id != -1)
	{
		res.isvar = true;
		res.local = res.constant = false;
		res.array = com->vm->vt.symbols[res.id].typeflags & IS_ARRAY;
		res.isfloat = com->vm->vt.symbols[res.id].typeflags & FLOAT_VAR;
		return res;
	}
	
	res.array = res.isvar = res.local = res.constant = false;
	res.id = -1;
	
	//TODO: write function search
	
	return res;
}

void panic(const char *s)
{
	//TODO: writeme
}
