// $Id$

/*
    CompSL scripting language 
    Copyright (C) 2007  Thomas Jones & John Peberdy

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "compiler/interncomp.h"

COMPSL_INTERN COMPSL_NONNULL COMPSL_INLINE expression *assignVar(const char *name, expression *ex) {
  return assignArray(name, NULL, ex);
}


// TODO: free mem
COMPSL_INTERN __attribute__((__nonnull__(1,3))) expression *assignArray(const char *name, bytecode *arrayIndex, expression *ex) {
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

  expr_autocast(var.isfloat,ex);
  expr_ensureLit(ex);
  len = bc_len(ex->val.bcode);

  DPRINTF("Assignent to variable \"%s\" with id=%i, local=%i, isfloat=%i\n", name, var.id, var.local, var.isfloat);
  if(var.array && arrayIndex!=NULL) {
    // Assignment of an array
    int cur = 0;
    int arLen = bc_len(arrayIndex);
    bytecode *mcode;

    cur = len;

    // Push the value
    mcode = realloc(ex->val.bcode, (len+arLen+3) * sizeof(bytecode));

    mcode[cur++].code = BC_DUP;

    // Push the array index
    memcpy(&mcode[cur], arrayIndex, arLen* sizeof(bytecode));
    cur +=arLen;

    // Do the assignment
    if(var.local)
      mcode[cur].code = BC_APOP;
    else
      mcode[cur].code = BC_GAPP;
    mcode[cur++].a1 = var.id;
    mcode[cur++].code = BC_NONO;
    
    assert(cur==len+arLen+3);

    ex->val.bcode = mcode; 

  } else if(!var.array && arrayIndex==NULL) {

    // Plain old assignment
    ex->val.bcode = realloc(ex->val.bcode, sizeof(bytecode)*(len+3));
    
    ex->val.bcode[len].code = BC_DUP;
    if(var.local)
      ex->val.bcode[len+1].code = BC_POP;
    else
      ex->val.bcode[len+1].code = BC_GPOP;
    ex->val.bcode[len+1].a1 = var.id;
    ex->val.bcode[len+2].code = BC_NONO;

  } else {
    sprintf(sprt,"Symbol \"%s\" used in wrong array context", name);
    compileError(sprt);
    return NULL;
  }
  return ex;
}


COMPSL_INTERN COMPSL_NONNULL COMPSL_INLINE expression *readVar(const char* name) {
  return readArray(name, NULL);
}

COMPSL_INTERN __attribute__((__nonnull__(1))) expression *readArray(const char* name, bytecode *arrayIndex) {
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
  bytecode *bc;
  if(var.array && arrayIndex!=NULL) {
    int len = bc_len(arrayIndex);
    // Array
    bc = calloc(len+2, sizeof(bytecode));
    if(ex == NULL || bc == NULL) internalCompileError("Out of Memory");
    memcpy(bc, arrayIndex, len*sizeof(bytecode));

    if(!var.local) 
      bc[len].code = BC_GAPS;
    else 
      bc[len].code = BC_APUSH;

    bc[len].a1 = var.id;
    bc[len+1].code = BC_NONO;

  } else if(!var.array && arrayIndex==NULL) {

    // Plain old variable
    bc = malloc(2*sizeof(bytecode));
    if(ex == NULL || bc == NULL) internalCompileError("Out of Memory");

    if(!var.local) 
      bc->code = BC_GPSH;
    else 
      bc->code = BC_PUSH;
        
    bc->a1 = var.id;
    bc[1].code = BC_NONO;
    
  } else {
    compileError("Array accessed in bad context");
    return NULL;
  }

  ex->val.bcode=bc;
  ex->isLiteral=false;
  ex->isFloat=var.isfloat;
  
  return ex;
}


COMPSL_INTERN COMPSL_NONNULL COMPSL_INLINE expression *incVar(const char *name, bool plus, bool post) {
  return incArray(name, NULL, plus, post);
}

COMPSL_INTERN __attribute__((__nonnull__(1))) expression *incArray(const char *name, bytecode *arrayIndex, bool plus, bool post) {
  expression *ex = malloc(sizeof(expression));
  bytecode *bc;
  symbolinfo var = searchSym(name,ccompart);
  int cpos = 0;
  int len = 5; // Basic length for variable increment

  if(ex == NULL) internalCompileError("Out of Memory");

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


  // Push the value onto the stack
  if (var.array && arrayIndex!=NULL) {
    int arLen = bc_len(arrayIndex);
    len+=arLen+2;
    bc = realloc(arrayIndex, len*sizeof(bytecode));
    if(bc == NULL) internalCompileError("Out of Memory");
    cpos+=arLen;

    bc[cpos++].code = BC_SAVE;

    if(!var.local) 
      bc[cpos].code = BC_GAPS;
    else 
      bc[cpos].code = BC_APUSH;

    bc[cpos].a1 = var.id;
    cpos++;

  } else if(!var.array && arrayIndex==NULL) {
    bc = calloc(len,sizeof(bytecode));
    if(bc == NULL) internalCompileError("Out of Memory");

    if(!var.local) 
      bc[cpos].code = BC_GPSH;
    else 
      bc[cpos].code = BC_PUSH;
    bc[cpos].a1 = var.id;
    cpos++;
  } else {
    sprintf(sprt,"Symbol \"%s\" used in wrong array context", name);
    compileError(sprt);
    return NULL;
  }


  // Increment or decrement the top of the stack
  {
    if(post)
      bc[cpos++].code = BC_DUP;
    
    if(var.isfloat)
      bc[cpos].code = ((plus)?BC_FINC:BC_FDEC);
    else
      bc[cpos].code = ((plus)?BC_INC:BC_DEC);
    cpos++;
    
    if(!post)
      bc[cpos++].code = BC_DUP; 
  }

  // Pop back into the var
  if(var.array && arrayIndex!=NULL) {
    // Array

    // Push the index
    bc[cpos++].code=BC_PSHT;

    // Do the assignment
    if(var.local)
      bc[cpos].code = BC_APOP;
    else
      bc[cpos].code = BC_GAPP;
    bc[cpos++].a1 = var.id;
        
  } else if(!var.array && arrayIndex==NULL) {
    if(!var.local)
      bc[cpos].code = BC_GPOP;
    else
      bc[cpos].code = BC_POP;
    bc[cpos].a1 = var.id;
    cpos++;
  } else {
    sprintf(sprt,"Symbol \"%s\" used in wrong array context", name);
    compileError(sprt);
    return NULL;
  }

  bc[cpos++].code = BC_NONO;

  assert(cpos==len);

  ex->val.bcode=bc;
  ex->isLiteral=false;
  ex->isFloat=var.isfloat;
  return ex;
}
