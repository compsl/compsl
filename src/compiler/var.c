#include <stdio.h>
#include "interncomp.h"

expression *assignVar(const char *name, expression *ex) {
  int len;
  
  symbolinfo var = searchSym(name,ccompart);
	  
  if(var.id<0) {
    sprintf(sprt,"Symbol \"%s\" not resolved",name);
    compileError(sprt);
    return NULL;
  }
	
  if(!var.isvar) {
    sprintf(sprt,"Symbol \"%s\" resolved to function, variable required", name);
    compileError(sprt);
    return NULL;
  }

  if(var.array) {
    sprintf(sprt,"Symbol \"%s\" resolved to array, not yet implemented", name);
    compileError(sprt);
    return NULL;
  }

  DPRINTF("Assignent to variable \"%s\" with id=%i, local=%i, isfloat=%i\n", name, var.id, var.local, var.isfloat);
  expr_autocast(var.isfloat,ex);
  expr_ensureLit(ex);
  len = bc_len(ex->val.bcode);
  ex->val.bcode = realloc(ex->val.bcode, sizeof(bytecode)*(len+3));
  
  ex->val.bcode[len].code = BC_DUP;
  if(var.local)
    ex->val.bcode[len+1].code = BC_POP;
  else
    ex->val.bcode[len+1].code = BC_GPOP;
  ex->val.bcode[len+1].a1 = var.id;
  ex->val.bcode[len+2].code = BC_NONO;
  
  return ex;
}


expression *readVar(const char* name) {
  symbolinfo var = searchSym(name,ccompart);
  
  if(var.id<0) {
    sprintf(sprt,"Symbol \"%s\" not resolved",name);
    compileError(sprt);
    return NULL;
  }
  
  if(!var.isvar) {
    sprintf(sprt,"Symbol \"%s\" is a function but is used in an assignment",name);
    compileError(sprt);
    return NULL;
  }
  expression *ex = malloc(sizeof(expression));
  bytecode *bc = malloc(2*sizeof(bytecode));
  if(ex == NULL || bc == NULL) internalCompileError("Out of Memory");
  if(!var.local) {
    bc->code = BC_GPSH;
    bc->a1 = var.id;
  }
  else {
    bc->code = BC_PUSH;
    bc->a1 = var.id;
  }
  bc[1].code = BC_NONO;
  ex->val.bcode=bc;
  ex->isLiteral=false;
  ex->isFloat=var.isfloat;
  
  return ex;
}
