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

#include <assert.h>
#include <stdlib.h>

#include "compiler/interncomp.h"


COMPSL_INTERN COMPSL_NONNULL int bc_len(bytecode* bc) {
  assert(bc!=NULL);
  int len=0;
  while(bc->code!=BC_NONO&&bc->code!=BC_END) {
    bc++;
    len++;
    if(len>10000) {
      internalCompileError("Non null terminated bytecode string");
      exit(4);
    }
  } 
  return len;
}


COMPSL_INTERN COMPSL_NONNULL void expr_ensureLit(expression* exp) {
  if(exp->isLiteral) {
    exp->val.bcode = expr_toBc(exp);
    exp->isLiteral = false;
  }
}

COMPSL_INTERN COMPSL_NONNULL void expr_autocast(bool toFloat,expression *e) {

  if(toFloat == e->isFloat) {
    // no cast needed

  } else if(e->isLiteral) {
    if( toFloat && !e->isFloat) {
      e->val.fl = (float)e->val.in;
      e->isFloat=true;
    }
    else {
      e->val.in = (int)e->val.fl;
      e->isFloat=false;
    } 
  } else {
    int len;
    len = bc_len(e->val.bcode);
    e->val.bcode = realloc(e->val.bcode, sizeof(bytecode)*(len+2));
    e->val.bcode[len].code = (toFloat && !e->isFloat)?BC_INFL:BC_FLIN;
    e->val.bcode[len+1].code = BC_NONO;
    e->isFloat = toFloat;
  }
}

/*
 * Note: doesnt free anything
 */
COMPSL_INTERN COMPSL_NONNULL bytecode* expr_toBc(expression *exp) {
  if(exp->isLiteral) {
    bytecode* bc = malloc(sizeof(bytecode)*2);
    if(bc==NULL) internalCompileError("Out of memory");
    bc[0].code=BC_CPUSH;
    
    intfloat tmp;
    if(exp->isFloat) {
	tmp.f = exp->val.fl;
    }
    else {
      tmp.i = exp->val.in;
    }
    bc[0].a1 = com_addConst(ccompart , tmp); 
    bc[1].code=BC_NONO;
    return bc;
  }
  else {
    if(exp->val.bcode==0) internalCompileError("Error in expr_toBc");
    return exp->val.bcode;
  }
}


COMPSL_INTERN COMPSL_NONNULL void expr_free(expression* expr) {
  if(!expr->isLiteral) {
    assert(expr->val.bcode != NULL);      
    free(expr->val.bcode);
    expr->val.bcode = NULL;
  }
  free(expr);
}
