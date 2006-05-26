#include <stdio.h>
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../extern/compsl.h"

int goparse(char* fn, compart *);

int main()
{
	printf("\nCompiler tests\nCompiling..\n");
	// fill this in
	
	const int NUM_FILES=2;
	
	char * files[] = 
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
		printf("Parsing: %s  ",files[i]);
		
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
