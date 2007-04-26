/*Tourture test for entire compsl system.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "../extern/compsl.h"

static const char *test2 = 
"declare \n\
{ \n\
   global int foo; \n\
   int x,y,z;\n\
}\n\
 cubbyhole init {\n\
 	 foo = 1;\n\
 }\
 cubby foo { \n\
 	x=0;\n\
 	while(x<3)\n\ 
 		x+=1;}\n\
 cubby bar {\n\
 	x=y=0;\n\ 
 	z = 1;\n\ 
 	x = y+z;\n\
  }";

int main()
{
	VM *vm=createVM();
	if(vm == NULL) { fprintf(stderr,"Error initializing compsl"); exit(1);}
	
	addPrintLibToVm(vm);
	addDebugLibToVm(vm);
	
	compart *com1=createComp(vm);
	compart *com2=createComp(vm);
	int compret1,compret2; // return code of compiler
	if(com1 == NULL || com2 == NULL)
	{
		fprintf(stderr,"Error initializing compsl");
		exit(1);
	}
	
	//TODO: add variables to compartments here
	int32_t *x = com_addInt(com1,"x");
	puts("Compiling file");
	compret1 = fileCompile("src/test/torture.csl", com1);
	puts("Compiling string");
	compret2 = stringCompile(test2, com2);
	if(compret1 != 0 || compret2 != 0) {
		fputs("Bad compsl!",stderr);
		exit(1);
	}
	int16_t cubbyid;
	
	cubbyid = getCubbyID(com1,"init");
	runCubbyhole(com1, cubbyid);
	puts("Done first init");
	
	cubbyid = getCubbyID(com2,"init");
	runCubbyhole(com2, cubbyid);
	puts("Done second init");
	
	//TODO: check value of foo global in cubbys
	cubbyid = getCubbyID(com1,"run");
	runCubbyhole(com1, cubbyid);
	//TODO: other tests
	
	cubbyid = getCubbyID(com1,"runlots");
	*x = 0;
	while(*x < 1000000) 
		runCubbyhole(com1, cubbyid);
}
