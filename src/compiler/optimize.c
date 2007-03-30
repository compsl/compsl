#include "interncomp.h"
#include "../intern/bytecode.h"

static void removeBytecode(bytecode *code, int ind, int codelen)
{
	for(int i = ind; i < codelen - 1; i++)
	{
		code[i] = code[i+1];
	}
}

bytecode *remUselessDUPs(bytecode *code, int codelen, VM *vm, compart * com)
{
	int sp = 0;
	
	for(int i = 0; i < codelen; i++)
	{
		if(code[i].code == BC_DPOP)
			// loop backwards and look for a DUP
			for(int j = i; j >= 0; j--)
				if(code[j].code == BC_DUP && stackcheck(&code[j],i - j,vm,com) == 0)
				{
					removeBytecode(code,i,codelen);
					removeBytecode(code,j,codelen);
					i-=2;
					break;
				}
	}
}