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
	const int NUM_FILES=9;
	
	const char * files[] = 
	{
		"src/test/base.csl",
		"src/test/debug.csl",
		"src/test/math.csl",
		"src/test/functions.csl",
		"src/test/decls.csl",
		"src/test/decls2.csl",
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
	    if(i==0 || i==6 || i==7)
	      printf("OK\n");
	  } else {
	    printf("\tCompile: FAIL with exit code %i\n", ret);
	  }
	}
	
	destroyVM(veem);
	
	return 0;
}


