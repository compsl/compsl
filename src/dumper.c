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

/*
 * A file program to compile and dump the resulting bytecode
 * also used to experiment with new functions for messing with
 * compiler output (optimizers, more complex stackcheckers etc.)
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "compsl.h"

#include "intern/gen.h"
#include "intern/bytecode.h"
#include "interp/bcstrings.h"
#include "intern/compartment.h"

static void dmp(compart *com, int id)
{	
	bytecode *pc= (bytecode *)(com->cubbys[id].code); // init program counter
	
	while(pc->code != BC_END && pc->code != BC_HLT && pc->code != BC_DBG)
	{
		long int tractmp = ((long int)pc - (long int)(com->cubbys[id].code))/sizeof(bytecode);
		printf("%4ld: %5s  sa=%- 8d  a1= %u\n",
			tractmp,tractbl[pc->code],pc->sa,pc->a1);
		pc++;
	}
}

const char *src =
"declare {int a,b,c,d; int stat; float xx;}\n\
cubby foo {\n\
	test_reset();\n\
        b=c=d=a=1;\n\
        testeqi(1,a++);\n\
        testeqi(2,a);\n\
        testeqi(3,++a);\n\
        testeqi(3,a);\n\
\n\
        testeqi(3,a--);\n\
        testeqi(2,a);\n\
}";

int main()
{
	VM *vm = createVM();
	if(vm == NULL) { fprintf(stderr,"Error initializing compsl"); exit(1);}
	
	addPrintLibToVm(vm);
	addDebugLibToVm(vm);
	
	compart *com = createComp(vm);
	int compret; // return code of compiler
	if(com == NULL)
	{
		fprintf(stderr,"Error initializing compsl");
		exit(1);
	}
	
	
	//compret = fileCompile("src/test/decls.csl",com2);
	compret = stringCompile(src,com);
	int16_t cubbyid = getCubbyID(com,"foo");
	dmp(com,cubbyid);
	
	destroyComp(com);	
	destroyVM(vm);
}
