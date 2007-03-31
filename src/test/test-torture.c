/*Tourture test for entire compsl system.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../extern/compsl.h"
#include "../extern/userspace.h"

#include "../intern/gen.h"
#include "../intern/bytecode.h"

static const char *test2 = 
"declare { global int foo; int x,y,z;}\
 cubbyhole init { foo = 1; }\
 cubby foo {x=0;while(x<3)x = x + 1;}\
 cubby bar {x=y=0; z = 1; x = y+z; }";

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
	compret1 = fileCompile("src/test/torture.csl", vm, com1);
	compret2 = stringCompile(test2, vm, com2);
	
	int16_t cubbyid;
	
#ifdef DEBUG
	cubbyid = getCubbyID(com2,"foo");
	dumpBytecode(com2,cubbyid);
	
	cubbyid = getCubbyID(com2,"bar");
	dumpBytecode(com2,cubbyid);
	int tmp =bc_len((bytecode *)(com2->cubbys[cubbyid].code));
	//for(;((bytecode *)(com2->cubbys[cubbyid].code))[tmp].code != BC_END; tmp++);
	remUselessDUPs(((bytecode *)(com2->cubbys[cubbyid].code)),tmp, com2, vm);
	dumpBytecode(com2,cubbyid);
#endif
	
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
