#include <stdio.h>
#include "../intern/compartment.h"
#include "../extern/compsl.h"
#include "../intern/vars.h"

// For DPRINTF
#include "../intern/debug.h"
#include "../compiler/node.h"

int main()
{
	const int NUM_FILES=12;
	
	const char * files[] = 
	{
		"src/test/base.csl",
		"src/test/debug.csl",
		"src/test/decls.csl",
		"src/test/decls2.csl",
		"src/test/math.csl",
		"src/test/functions.csl",
		"src/test/expressions.csl",
		"src/test/casting.csl",
		"src/test/flow.csl",
		"src/test/mincrash.csl",
		"src/test/comparison.csl",
		"src/test/arrays.csl"
	};
	
	VM *veem = createVM();
	compart *com[NUM_FILES];
	addPrintLibToVm(veem);
	addDebugLibToVm(veem);
	
	for(int i=0;i<NUM_FILES;i++) {
	  int ret;
	  printf("File %s: ",files[i]);
	  
	  com[i] = createComp(veem);
	  ret = fileCompile(files[i], veem, com[i]);
	  if(ret==0){
	    printf("\tCompile: OK, Run: ");
	    runCubbyhole(com[i], 0); 
	    if(i==0)
	      printf("OK\n");
	  } else {
	    printf("\tCompile: FAIL with exit code %i\n", ret);
	  }
	  destroyComp(com[i]);
	}
	
	destroyVM(veem);
	
	return 0;
}

