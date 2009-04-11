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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler/node.h"
#include "compiler/interncomp.h"

#include "compsl.tab.h"



/*
 * Binary operator helpers for bin_op()
 * Returns the bytecode corresponding to 'a <op> b'
 *
 * Constraint: The +=, *=, ..= operators on arrays use the scratch register to store array indexes
 * This happens near the end of b -> Caller assumes we won't write the scretch register between b
 * and the end of the returned bytecode string.
 */
COMPSL_INTERN expression* bin_bc_op(int op,expression* a, expression* b) {
  assert((a!=(expression*)0) && (b!=(expression*)0));
  bool isFloat;
  int alen,blen, endi;
  bytecode* mcode;

  expr_ensureLit(a);
  expr_ensureLit(b);

  // Cast to float if either are floats
  if(!a->isFloat && !b->isFloat) {
    isFloat = false;
  } else {
    expr_autocast(true, a);
    expr_autocast(true, b);
    isFloat = true;
  }

  alen = bc_len(a->val.bcode);
  blen = bc_len(b->val.bcode);
  endi = alen+blen;

  mcode = calloc(alen+blen+2,sizeof(bytecode));
  if(mcode == NULL) internalCompileError("Out of Memory");

  memcpy(mcode, a->val.bcode,sizeof(bytecode)*alen);
  memcpy(&mcode[alen], b->val.bcode,sizeof(bytecode)*blen);

  switch(op) {
  case PLUS:
    if(isFloat) mcode[endi].code = BC_FADD;
    else mcode[endi].code = BC_ADD;
    break;
  case MINUS:
    if(isFloat) mcode[endi].code = BC_FSUB;
    else mcode[endi].code = BC_SUB;
    break;
  case MULT:
    if(isFloat) mcode[endi].code = BC_FMUL;
    else mcode[endi].code = BC_MUL;
    break;
  case DIV:
    if(isFloat) mcode[endi].code = BC_FDIV;
    else mcode[endi].code = BC_DIV;
    break;
  case MOD:
    if(isFloat) mcode[endi].code = BC_FMOD;
    else mcode[endi].code = BC_MOD;
    break;
  case OR:
    mcode[endi].code = BC_OR;
    break;
  case AND:
    mcode[endi].code = BC_AND;
    break;
  case ISEQ:
    if(isFloat) mcode[endi].code = BC_FEQ;
    else mcode[endi].code = BC_EQ;
    break;
  case ISNEQ:
    if(isFloat) mcode[endi].code = BC_FNE;
    else mcode[endi].code = BC_NE;
    break;
  case ISGEQ:
    if(isFloat) mcode[endi].code = BC_FGE;
    else mcode[endi].code = BC_GE;
    break;
  case ISGT:
    if(isFloat) mcode[endi].code = BC_FGR;
    else mcode[endi].code = BC_GR;
    break;
  case ISLEQ:
    if(isFloat) mcode[endi].code = BC_FLE;
    else mcode[endi].code = BC_LE;
    break;
  case ISLT:
    if(isFloat) mcode[endi].code = BC_FL;
    else mcode[endi].code = BC_LS;
    break;
  case BITWISE_OR:
    if(isFloat) compileWarning("Doing bitwise OR of floats");
    mcode[endi].code = BC_BOR;
    break;
  case BITWISE_AND:
    if(isFloat) compileWarning("Doing bitwise AND of floats");
    mcode[endi].code = BC_BAND;
    break;
  case SHIFT_LEFT:
    if(isFloat) compileWarning("Doing left shift of a float");
    mcode[endi].code = BC_SFTL;
    break;
  case SHIFT_RIGHT:
    if(isFloat) compileWarning("Doing right shift of a float");
    mcode[endi].code = BC_SFTR;
    break;
  default:
    free(mcode);
    return NULL;
  }
  mcode[endi].a1 = 0;
  mcode[endi+1].code = BC_NONO;
  mcode[endi+1].a1=0;

  free(a->val.bcode);
  a->val.bcode = NULL;

  expr_free(b);
  a->val.bcode = mcode;
  return a;
}

// OPTIMIZE TODO: add more operators in bin_lit_op - would precalculate more stuff at compile time
COMPSL_INTERN expression* bin_lit_op(int op, expression* a, expression* b) {
  if(!a->isFloat && !b->isFloat) {
    int n1 = a->val.in;
    int n2 = b->val.in;
    switch(op){
    case PLUS:
      n1+=n2; break;
    case MINUS:
      n1-=n2; break;
    case MULT:
      n1*=n2; break;
    case DIV:
      n1/=n2; break;
    default:
      return NULL;
    }
    a->val.in = n1;
    free(b);
    b=NULL;
    return a;
  }
  else {
    float n1 = (a->isFloat)?a->val.fl:((float)a->val.in);
    float n2 = (b->isFloat)?b->val.fl:((float)b->val.in);
    switch(op) {
    case PLUS:
      n1+=n2; break;
    case MINUS:
      n1-=n2; break;
    case MULT:
      n1*=n2; break;
    case DIV:
      n1/=n2; break;
    default:
      return NULL;
    }
    a->isFloat=true;
    a->val.fl=n1;
    free(b); b = NULL;
    return a;
  }
}


// Post: dont use a or b, they're freed if need be

COMPSL_INTERN expression* bin_op(int op,expression* a, expression* b) {
  assert((a!=NULL) && (b!=NULL));
  expression *ret;
  if(a->isLiteral && b->isLiteral) {
    ret = bin_lit_op(op,a,b);
    if(NULL==ret)
      ret = bin_bc_op(op,a,b);
  } else {
    ret = bin_bc_op(op,a,b);
  }
  if(NULL==ret) {
    internalCompileError("Binary operation not implemented");
  }
  return ret;
}
