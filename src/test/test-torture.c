/*Tourture test for entire compsl system.
 *
 */

#include <stdio.h>
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../extern/compsl.h"
#include "../extern/userspace.h"

static const char *test2 = 
"declare { global int foo; }\
 cubbyhole init { foo = 1; }";

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
	
	compret2 = stringCompile(test2, vm, com2);
	compret1 = fileCompile("src/test/torture.csl", vm, com1);
	
	//TODO: run init cubby on both in order here.
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
}
