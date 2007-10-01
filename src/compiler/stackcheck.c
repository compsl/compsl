// $Id$

/*
    CompSL scripting language 
    Copyright (C) 2007  Thomas Jones & John Peberdy

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "interncomp.h"
#include "intern/gen.h"


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
	for(int i = 0; i < codelen && i > -1; i++)
	{
		if(	((BC_POP <= code[i].code && code[i].code <= BC_GAPP) ||
			(BC_ADD <= code[i].code && code[i].code <= BC_FGE) || 
			code[i].code == BC_JMZ || code[i].code == BC_JMN) 
			&& code[i].code != BC_NOT && code[i].code != BC_BNOT)
		{ // move stack pointer backwards 1 (pop)	
			sp--;

			// Array ones take two params
			if(code[i].code==BC_GAPP || code[i].code==BC_APOP)
			  sp--;

			if(sp < 0){
				badStack = true;
				fprintf(stderr,"Stack underflow at %d instrunctions into expression on line %d, underflow by %d\n", 
					i, lineNo, sp);
			}
		}
		else if((BC_PUSH <= code[i].code && code[i].code <= BC_GAPS) ||
			 	code[i].code == BC_DUP) 
				
		{ // move stack pointer forwards 1 (push)
			sp++;
			if(code[i].code==BC_GAPS || code[i].code==BC_APUSH)
			  sp--;
			

			if(sp >= VM_STACK_SIZE){
				badStack = true;
				fprintf(stderr,"Stack overflow at %d instrunctions into expression on line %d, overflow by %d\n", 
					i, lineNo, sp - VM_STACK_SIZE + 1);
			}
		}
		else if(code[i].code == BC_CALL)
		{ // function call
			if(code[i].a1 >= vm->ncnt) {
				fprintf(stderr,"BAD NATIVE CALL! instruction %d of expression on line %d", i, lineNo);
				internalCompileError("BAD NATIVE CALL GENERATED!");
			}
			
			sp -= vm->natives[code[i].a1].numParam;
			if(sp < 0) {
				badStack = true;
				fprintf(stderr,"Stack underflow at %d instrunctions into expression on line %d, underflow by %d\n", 
					i, lineNo, sp);
			}
			
			sp++;
			
			if(sp >= VM_STACK_SIZE){
				badStack = true;
				fprintf(stderr,"Stack overflow at %d instrunctions into expression on line %d, overflow by %d", 
					i, lineNo, sp - VM_STACK_SIZE + 1);
			}
		}
		else if(BC_ABS <= code[i].code && code[i].code <= BC_FEQUAL)
		{ // builtin
			int j;
			for(j = 0; j < builtins_len && builtins[j].code != code[i].code; j++);
			sp -= builtins[j].ac; 
			if(sp < 0) {
				badStack = true;
				fprintf(stderr,"Stack underflow at %d instrunctions into expression on line %d, underflow by %d\n", 
					i, lineNo, sp);
			}
			sp++;
			
		}
		else if(code[i].code == BC_JMP)
		{ // gotta follow it!
			i += code[i].sa -1;
			return sp;
		}
		else if(code[i].code == BC_END || code[i].code == BC_DBG || code[i].code == BC_HLT)
		{
			return sp;
		}
		else if(BC_NOOP > code[i].code || code[i].code > BC_DBG)
		{
			internalCompileError("BAD OPCODE found while checking stack");
		}
	}
	if(badStack)
	{
		dumpBytecode2(com,code);
		internalCompileError("Stack bounds exceeded");
	}
	return sp;
}
