
#include "interncomp.h"
#include <stdlib.h>
#include <stdio.h>

void compileError(const char *str) {
  fflush(stderr);
  fflush(stdout);
  fprintf(stderr,"Compile error: %s\n",str);
}

 void compileWarning(const char *str) {
  fflush(stderr);
  fflush(stdout);
  fprintf(stderr,"Compile warning: %s\n",str);
 }

char foo[1024];

void internalCompileError(const char* str) {
	DPRINTF("internal Compile error\n");
  sprintf(foo,"INTERNAL ERROR: %s\n",str);
  compileError(foo);	
  abort();
}
