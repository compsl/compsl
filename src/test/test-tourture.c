/*Tourture test for entire compsl system.
 *
 */

#include <stdio.h>
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../extern/compsl.h"
#include "../extern/userspace.h"

int main()
{
	VM *vm=createVM();
	compart *com1=createComp();
	compart *com2=createComp();
	int compret1,compret2; // return code of compiler
	if(vm == NULL || com1 == NULL || com2 == NULL)
	{
		fprintf(stderr,"Error initializing compsl");
	}
	
	compret1 = fileCompile("../src/test/tourture.csl", vm, com1);
	compret2 = fileCompile("../src/test/torture2.csl", vm, com1);
	
}