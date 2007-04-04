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
#include "extern/compsl.h"

#include "intern/gen.h"
#include "intern/bytecode.h"

#ifdef DEBUG
#define DUMP(com,id) dumpBytecode((com), (id));
#else
#define DUMP(com,id) ;
#endif

const char *src =
"declare {int a,b,c,d; int stat;}\n\
 cubby foo {\n\
 	a=b=c=0;\n\
 	d=3;\n\
 	if(a < 1)\n\
 	{\n\
 		sqrt(d);\n\
 	} else\n\
 		pow(d,2);\n\
 	while(a < 3) {\n\
 		if(b<a)\n\
 			c=a + b;\n\
 		else\n\
 			c= a - b;\n\
 		a = a + 1;\n\
 	}\n\
 }\n\
 cubby pp { a=2; testeqi(2,a++);}\n\
 cubby bar { \n\
 	    a=8;\n\
	while ( 1+a) {\n\
        a = a++;\n\
        if(a-16) continue;\n\
		break;\n\
	}\n\
 }\n\
 cubby baz {\n\
 	a=5;\n\
	while (a) {\n\
    	a = a - 1;\n\
    	b=3;\n\
    	while(b) b = b-1;\n\
	}\n\
}";

int main()
{
	VM *vm=createVM();
	if(vm == NULL) { fprintf(stderr,"Error initializing compsl"); exit(1);}
	
	addPrintLibToVm(vm);
	addDebugLibToVm(vm);
	
	compart *com=createComp(vm);
	int compret; // return code of compiler
	if(com == NULL)
	{
		fprintf(stderr,"Error initializing compsl");
		exit(1);
	}
	
	compret = stringCompile(src, vm, com);
	
	int16_t cubbyid = getCubbyID(com,"foo");
	if(cubbyid < 0) {fprintf(stderr,"WTF?"); exit(1); }
	dumpBytecode(com,cubbyid);
	
	printf("\nSecond\n\n");
	
	cubbyid = getCubbyID(com,"bar");
	if(cubbyid < 0) {fprintf(stderr,"WTF?"); exit(1); }
	dumpBytecode(com,cubbyid);
	
	printf("\nThird\n\n");
	
	cubbyid = getCubbyID(com,"baz");
	if(cubbyid < 0) {fprintf(stderr,"WTF?"); exit(1); }
	dumpBytecode(com,cubbyid);
	
	printf("\nINC\n\n");
	
	cubbyid = getCubbyID(com,"pp");
	if(cubbyid < 0) {fprintf(stderr,"WTF?"); exit(1); }
	dumpBytecode(com,cubbyid);
	
	
//	setvbuf(stdout,NULL,_IONBF,0);
//	runCubbyhole(com,cubbyid);
	
	destroyComp(com);
//	printf("Tring flow-control");
//	com = createComp(vm);
//	if(com == NULL)
//	{
//		fprintf(stderr,"Error initializing compsl");
//		exit(1);
//	}
//	compret = fileCompile("src/test/flow.csl", vm, com);
//	
//	cubbyid = getCubbyID(com,"init");
//	if(cubbyid < 0) {fprintf(stderr,"WTF?"); exit(1); }
//	dumpBytecode(com,cubbyid);
//	runCubbyhole(com,cubbyid);
	
	destroyVM(vm);
}
