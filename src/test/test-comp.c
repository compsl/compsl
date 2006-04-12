#include "../compiler/compsl.tab.h"
#include <stdio.h>

int main()
{
	printf("\nRunning compiler tests\n");
	// fill this in
	
	char ** files[5][40];
	strcpy(files[0], "src/test/base-structure.csl");
	strcpy(files[1], "src/test/casting.csl");
	strcpy(files[2], "src/test/declarations.csl");
	strcpy(files[3], "src/test/expressions.csl");
	strcpy(files[4], "src/test/flow-control.csl");
	  
	  
	for(int i=0;i<5;i++) {
		printf("Parsing: %s  ",files[i]);
		fflush(stdout);
		goparse(files[i]);
		printf(":-)\n");
	}
	
	return 0;
}
