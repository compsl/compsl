
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


void internalCompileError(const char* str) {
  sprintf(sprt,"INTERNAL ERROR: %s",str);
  compileError(sprt);	
  exit(1);
}
