#include <stdio.h>
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../extern/compsl.h"
#include "../intern/vars.h"

int goparse(char* fn, compart *);

void addPrint(VM *vm); // add a printi(int) and printf(float) functions to the vm 

int main()
{
	printf("\nCompiler tests\nCompiling..\n");
	// fill this in
	
	const int NUM_FILES=5;
	
	const char * files[] = 
	{
		"src/test/base-structure.csl",
		"src/test/print-for-dbg.csl",
		"src/test/addition.csl",
		"src/test/declarations.csl",
		"src/test/expressions.csl",
		"src/test/casting.csl",
		"src/test/flow-control.csl"
	};
	  
	  
	VM *veem = createVM();
	compart *com[NUM_FILES];

	for(int i=0;i<NUM_FILES;i++) {
		printf("Compiling: %s  ",files[i]);
		
		com[i] = createComp(veem);
		fflush(stdout);
		goparse(files[i], com[i]);

		printf("\t:-)");
		
		printf("\t< "); 
		com_prStats(com[i]);
		printf(">\n");
		

	}
	
	puts("\nRunning some cubbys..");
	
	runCubbyhole(com[1], 0); 
	
	//TODO free com's
	
	destroyVM(veem);
	
	return 0;
}

intfloat compsl_printFloat(var *args) { printf("Value: %f", args[0].v.f); return (intfloat)0;}
intfloat compsl_printInt(var *args) { printf("Value: %i", args[0].v.i); return (intfloat)0;}	

void addPrint(VM *vm)
{
	// add printf
	vm->natives[0].func= &compsl_printFloat;
	vm->natives[0].numParam = 1;
	vm->natives[0].params = malloc(sizeof(var));
	vm->natives[0].paramFlags = malloc(sizeof(uint8_t));
	vm->natives[0].paramFlags[0]=FLOAT_VAR;
	vm->natives[0].isVoid=true;
	vm->natives[0].name = "printf";

	//add printi
	vm->natives[1].func= &compsl_printInt;
	vm->natives[1].numParam = 1;
	vm->natives[1].params = malloc(sizeof(var));
	vm->natives[1].paramFlags = malloc(sizeof(uint8_t));
	vm->natives[1].paramFlags[0]=0;
	vm->natives[1].isVoid=true;
	vm->natives[1].name = "printi";
}
