#include "interncomp.h"
#include "../intern/bytecode.h"
#include "../intern/gen.h"
#include <stdio.h>

static void removeBytecode(bytecode *code, int ind, int codelen)
{
	for(int i = ind; i < codelen - 1; i++)
	{
		code[i] = code[i+1];
	}
}

static int stackpos(const bytecode *code, int codelen, VM *vm, compart * com)
{
	int sp = 0;
	bool badStack=false;
	for(int i = 0; i < codelen && i > -1; i++)
	{
		if(sp < 0) return sp;
		
		if(	((BC_POP <= code[i].code && code[i].code <= BC_GAPP) ||
			(BC_ADD <= code[i].code && code[i].code <= BC_FGE) || 
			code[i].code == BC_JMZ || code[i].code == BC_JMN) 
			&& code[i].code != BC_NOT && code[i].code != BC_BNOT)
		{ // move stack pointer backwards 1 (pop)	
			sp--;
		}
		else if((BC_PUSH <= code[i].code && code[i].code <= BC_GAPS) ||
			 	code[i].code == BC_DUP) 
				
		{ // move stack pointer forwards 1 (push)
			sp++;
		}
		else if(code[i].code == BC_CALL)
		{ // function call
			if(code[i].a1 >= vm->ncnt)
			{
				fprintf(stderr,"BAD NATIVE CALL! instruction %d of expression on line %d", i, lineNo);
				internalCompileError("BAD NATIVE CALL GENERATED!");
			}
			
			sp -= vm->natives[code[i].a1].numParam;
			sp++;
		}
		else if(BC_ABS <= code[i].code && code[i].code <= BC_HYPOT)
		{ // builtin
			int j;
			for(j = 0; j < builtins_len && builtins[j].code != code[i].code; j++);
			sp -= builtins[j].ac; sp++;
			
		}
		else if(code[i].code == BC_JMP)
		{ // gotta follow it!
			//i += code[i].sa -1;
			return sp;
		}
		else if(code[i].code == BC_END || code[i].code == BC_DBG || code[i].code == BC_HLT)
		{
			return sp;
		}
		else if(BC_NOOP > code[i].code || code[i].code > BC_DBG)
		{
			internalCompileError("BAD OPCODE!!!!!");
		}
	}
	return sp;
}
bytecode *remUselessDUPs(bytecode *code, int codelen, VM *vm, compart * com)
{
	for(int i = 0; i < codelen; i++)
	{
		if(code[i].code == BC_DPOP)
			// loop backwards and look for a DUP
			for(int j = i; j >= 0; j--)
				if(code[j].code == BC_DUP && stackpos(&code[j],i - j,vm,com) == 0)
				{
					removeBytecode(code,i,codelen);
					removeBytecode(code,j,codelen);
					i-=2;
					break;
				}
	}
	return code;
}
