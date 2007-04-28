#include <stdio.h>
#include "../intern/compartment.h"
#include "../extern/compsl.h"
#include "../intern/vars.h"

// For DPRINTF
#include "../intern/debug.h"
#include "../compiler/node.h"
#include "../compiler/interncomp.h"

#if defined(WIN32) || defined(DJGPP)
#define BIT_BUCKET "NUL"
#else
#define BIT_BUCKET "/dev/null"
#endif


int main()
{
	puts("Running Compiler Tests");
	
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
		"src/test/arrays.csl",
		NULL
	};
	
	const char *err_files[] =
	{
		"src/test/missingDec.csl",
		"src/test/error.csl",
		NULL
	};
	
	VM *veem = createVM();
	compart *com[NUM_FILES];
	addPrintLibToVm(veem);
	addDebugLibToVm(veem);
	
	for(int i=0;files[i] != NULL;i++) {
	  int ret;
	  printf("File %s: ",files[i]);
	  
	  com[i] = createComp(veem);
	  ret = fileCompile(files[i], com[i]);
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
	
	puts("Begging error testing");
	comp_out=fopen(BIT_BUCKET,"w");
	
	VM *evm = createVM();
	compart *ecom=NULL;
	
	if(evm == NULL) 
	{
		puts("Error creating VM");
		return 1;
	}
	
	for(int i=0;err_files[i] != NULL;i++) {
		int ret;
		printf("File %s: ",err_files[i]);
		  
		ecom = createComp(evm);
		if(ecom  == NULL) 
		{
			printf("Error creating compartment");
			return 1;
		}
	  	ret = fileCompile(err_files[i], ecom);
	  	if(ret==0){
	    	printf("\tFAIL\n");
	    	return 2;
	  	} else {
	    	printf("\tPASS\n");
	  	}
	  	destroyComp(ecom);
	  	ecom = NULL;
	}
	
	
	
	return 0;
}

