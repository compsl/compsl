#include <stdio.h>
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../intern/compartment.h"
#include "../extern/compsl.h"
#include "../intern/vars.h"
#include "../extern/userspace.h"

// For DPRINTF
#include "../intern/debug.h"
#include "../compiler/node.h"

int main()
{
	printf("\nCompiler tests\n");
	const int NUM_FILES=6;
	
	const char * files[] = 
	{
		"src/test/base.csl",
		"src/test/debug.csl",
		"src/test/math.csl",
		"src/test/functions.csl",
		"src/test/decls.csl",
		"src/test/expressions.csl",
		"src/test/casting.csl",
		"src/test/flow-control.csl"
	};
	  
	  
	VM *veem = createVM();
	compart *com[NUM_FILES];
	addPrintLibToVm(veem);
	addDebugLibToVm(veem);
	
	for(int i=0;i<NUM_FILES;i++) {
	  int ret;
	  printf("File %s: ",files[i]);

	  ret = fileCompile(files[i], veem, &com[i]);
	  if(ret==0){
	    printf("\tCompile: OK, Run: ");
	    runCubbyhole(com[i], 0); 
	    if(i==0||i==4 || i==5)
	      printf("OK\n");
	  } else {
	    printf("\tCompile: FAIL with exit code %i\n", ret);
	  }
	}
	
	/*        printf("\nRunning %s - ", files[1]);
	runCubbyhole(com[1], 0); 

        printf("Running: %s - ", files[2]);
	runCubbyhole(com[2], 0); 	

        printf("Running: %s - ", files[3]);
	runCubbyhole(com[3], 0); 	
	*/

	//TODO? free com's
	
	destroyVM(veem);
	
	return 0;
}


