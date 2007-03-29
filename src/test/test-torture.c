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
 init { foo = 1; }";

int main()
{
	VM *vm=createVM();
	if(vm == NULL) { fprintf(stderr,"Error initializing compsl"); exit(1);}
	
	compart *com1;//=createComp(vm);
	compart *com2=createComp(vm);
	int compret1,compret2; // return code of compiler
	if(com1 == NULL || com2 == NULL)
	{
		fprintf(stderr,"Error initializing compsl");
		exit(1);
	}
	
	//TODO: add variables to compartments here
	
	compret1 = fileCompile("../src/test/torture.csl", vm, &com1);
	compret2 = stringCompile(test2, vm, com1);
	
	//TODO: run init cubby on both in order here.
	//TODO: check value of foo global in cubbys
	//TODO: other tests
}
