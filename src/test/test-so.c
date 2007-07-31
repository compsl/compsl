#include <stdio.h>
#include <stdlib.h>
#include "../extern/compsl.h"

static const char *test = 
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
	puts("Running Shared Library Test");
	
	VM *vm=createVM();
	if(vm == NULL) { fprintf(stderr,"Error initializing compsl"); exit(1);}
	
	addPrintLibToVm(vm);
	addDebugLibToVm(vm);
	
	compart *com1=createComp(vm);
	int compret;
	
	if(com1 == NULL)
	{
		fprintf(stderr,"Error initializing compsl");
		exit(1);
	}
	//TODO: add variables to compartments here
	puts("Compiling string");
	compret = stringCompile(test, com1);
	
	if(compret != 0) {
		fputs("Bad compsl!",stderr);
		exit(1);
	}
	
	int16_t cubbyid;
	cubbyid = getCubbyID(com1,"init");
	runCubbyhole(com1, cubbyid);
	puts("PASS");
	destroyComp(com1);
	destroyVM(vm);
}
