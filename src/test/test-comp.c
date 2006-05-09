#include <stdio.h>

int main()
{
	printf("\nRunning compiler tests\n");
	// fill this in
	
	char * files[] = 
	{
		"src/test/base-structure.csl",
		"src/test/casting.csl",
		"src/test/declarations.csl",
		"src/test/addition.csl",
		"src/test/expressions.csl",
		"src/test/flow-control.csl"
	};
	  
	for(int i=0;i<5;i++) {
		printf("Parsing: %s  ",files[i]);
		fflush(stdout);
		goparse(files[i]);
		printf(":-)\n");
	}
	
	return 0;
}
