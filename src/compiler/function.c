
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "interncomp.h"

expression *function_call(const char* name, list *params) {
  expression *ex = malloc(sizeof(expression));
  if(ex == NULL) internalCompileError("Out of Memory");
  bytecode *mcode;
  ex->isFloat = false;
  int lenBc, curBc;
  
  bytecode callcode;
  int numParams=0;
  uint8_t *paramFlags = 0;
  bool freeParamFlags = false;
  bool found=false;
  
  llist *curParam;	  
  
  // PYES bytecode
  if(strcmp(name,"yes")==0) {
    callcode.code = BC_PYES;
    callcode.a1 =1;
    found = true;
  } else if(strcmp(name,"no")==0) {
    callcode.code = BC_PYES;
    callcode.a1 =0;
    found = true;
  }
  
  // Built in funcs
  if(!found) {
    for(int i=0;i<builtins_len;i++) {
      if(strcmp(name,builtins[i].name)==0) {
	DPRINTF("Function \"%s\" found is a built in function\n",name);
	bool isFloat = builtins[i].isFloat;
	callcode.code = builtins[i].code;
	callcode.a1 =0;
	numParams = builtins[i].ac; 
	ex->isFloat = builtins[i].isFloat;
	paramFlags = malloc(sizeof(uint8_t)*numParams);
	freeParamFlags = true;
	for(int q=0;q<numParams;q++)
	  paramFlags[q] = ((isFloat)?FLOAT_VAR:0);
	found = true;
	break;
      }
    }
  }
  
  // Native function
  if(!found) {
    symbolinfo symbol = searchSym(name,ccompart);
    nativeFN *funk;
    if(symbol.id<0) {
      sprintf(sprt, "Function %s does not exist",name);
      compileError(sprt);
      return NULL;
    } else if(symbol.isvar) {
      sprintf(sprt,"Variable %s used as a function call",name);
      compileError(sprt);
      return NULL;
    }
    funk = &ccompart->vm->natives[symbol.id];
    paramFlags = funk->paramFlags;
    ex->isFloat = funk->retFloat;
    numParams = funk->numParam;
    callcode.code = BC_CALL;
    callcode.a1 = symbol.id;
    found = true;
    
    DPRINTF("Function \"%s\" found with id %i, function=%p\n",name,symbol.id, funk->func);
    assert(strcmp(name,funk->name)==0); 
    assert(funk->func!=0);
  }	    
  
  assert(found);
  assert(paramFlags!=0 || numParams==0);
  
  
  // Check no. arguments
  if(params->length != numParams) {
    sprintf(sprt, "Function %s has %i parameters but %i found",name, numParams, params->length);
    compileError(sprt);
    return 0;
  }
  
  // 1 for callcode, 1 for BC_NONO
  lenBc = 2;
  
  // Ready the parameters and count the size of the bytecodes
  curParam = params->head;
  for(int i=0; i< numParams;i++) {
    expression* ce = (expression*)curParam->obj;
    expr_autocast(paramFlags[i] & FLOAT_VAR, ce);
    
    if(paramFlags[i] & IS_ARRAY) {
      sprintf(sprt, "Function %s has parameters %i as an array, however array wasn't found",name, i);
      compileError(sprt);
      return NULL;
    }
    
    if(ce->isLiteral) {
      ce->val.bcode = expr_toBc(ce);
      ce->isLiteral = false;
    }
    lenBc+=bc_len(ce->val.bcode);
    curParam = curParam->next;
    DPRINTF("  Parameter %i has bytecode length %i, new total is %i\n",i, bc_len(ce->val.bcode), lenBc);
  }
  
  mcode = calloc(lenBc, sizeof(bytecode));
  if(mcode == NULL) internalCompileError("Out of Memory");
  curBc = 0;
  
  // Copy the parameters into mcode, last parameter first
  for(int i=numParams-1;i>=0;i--) {
    expression* ce = (expression*)list_get(params,i); // inneficient
    int clen = bc_len(ce->val.bcode);
    memcpy(&mcode[curBc],ce->val.bcode,sizeof(bytecode)*clen);
    expr_free(ce);
    curBc+=clen;
  }
  
  mcode[curBc] = callcode;
  curBc++;
  mcode[curBc].code = BC_NONO;
  curBc++;
  assert(curBc==lenBc);
  
  ex->isLiteral = false;
  ex->val.bcode = mcode;
  
  if(freeParamFlags) free(paramFlags);
  list_free(params);
  // params contents already free'd in copy loop
  
  DPRINTF("Function call completed with length %i with isFloat=%i\n", bc_len(ex->val.bcode), ex->isFloat);
  
  return ex;
}
