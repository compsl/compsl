#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "interncomp.h"


/* stackcheck: 
 * 	check code generated by compiler for stack errors
 * 
 * code = code to check
 * codelen = number of bytecodes in code.
 * vm = vm to compile with
 * com = compartment code is compilling with
 * 
 * return value: stack offest at end of code.
 */
int stackcheck(const bytecode *code, int codelen, VM *vm, compart * com)
{
	int sp = 0;
	bool badStack=false;
	for(int i = 0; i < codelen; i++)
	{
		if((BC_PUSH <= code[i].code && code[i].code <= BC_GAPS) || code[i].code == BC_DUP)
		{ // move stack pointer backwards 1 (pop)	
			sp--;
			if(sp < 0){
				badStack = true;
				fprintf(stderr,"Stack underflow at %d instrunctions into expression on line %d, underflow by %d\n", i, lineNo, sp);
			}
		}
		else if((BC_POP <= code[i].code && code[i].code <= BC_GAPP) || 
				(BC_ADD <= code[i].code && code[i].code <= BC_FGE) || 
				code[i].code == BC_JMZ || code[i].code == BC_JMN)
		{ // move stack pointer forwards 1 (push)
			sp++;
			if(sp >= VM_STACK_SIZE){
				badStack = true;
				fprintf(stderr,"Stack overflow at %d instrunctions into expression on line %d, overflow by %d\n", i, lineNo, sp - VM_STACK_SIZE + 1);
			}
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
			if(sp < 0){
				badStack = true;
				fprintf(stderr,"Stack underflow at %d instrunctions into expression on line %d, underflow by %d\n", i, lineNo, sp);
			}
			else if(sp >= VM_STACK_SIZE){
				badStack = true;
				fprintf(stderr,"Stack overflow at %d instrunctions into expression on line %d, overflow by %d", i, lineNo, sp - VM_STACK_SIZE + 1);
			}
		}
		else if(BC_ABS <= code[i].code && code[i].code <= BC_HYPOT)
		{ // builtin
			int j;
			for(j = 0; j < builtins_len && builtins[j].code != code[i].code; j++);
			sp -= builtins[j].ac; sp++;
			
		}
		else if(code[i].code == BC_JMP)
		{ // gotta follow it!
			i += code[i].sa -1;
		}
		else if(code[i].code == BC_END || code[i].code == BC_DBG || code[i].code == BC_HLT)
		{
			return sp;
		}
		else if(BC_NOOP < code[i].code || code[i].code > BC_DBG)
		{
			internalCompileError("BAD OPCODE!!!!!");
		}
	}
	if(badStack)
		internalCompileError("Stack bounds exceeded");
	return sp;
}
