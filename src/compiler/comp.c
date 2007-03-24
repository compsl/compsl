#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "node.h"
#include "../extern/compart.h"
#include "../intern/compartment.h"
#include "../intern/vars.h"

compart *ccompart;


void internalCompileError(const char* str) {
	sprintf(sprt,"Internal error: %s",str);
	compileError(sprt);	
	exit(1);
}

list* list_new(void) {
	list *lst = malloc(sizeof(list));
	lst->length=0;
	lst->head=0;
	return lst;
}

// gets the ind'th element in lst
void* list_get(list * lst, int ind) {
	llist *root = lst->head;
	while(root!=0 && ind-- > 0) {
		root=root->next;
	}
	if(root!=0) { 
		if(ind>=lst->length) { internalCompileError("Internal error in list_get"); }
		return root->obj;
	}
	return 0;
}

// Note: contents aren't freed
void llist_free(llist *lst) {
  if(lst==(llist*)0) return;
  llist *next = lst->next;
  free(lst);
  llist_free(next);
}	

// Note, contents aren't freed
void list_free(list *lst){
  llist_free(lst->head);
  free(lst);
}

void list_addToFront(list *lst, void *newOb) {
	llist *newL = malloc(sizeof(llist));
	newL->obj=newOb;
	newL->next=lst->head;
	lst->head=newL;
	lst->length++;
}


void* list_popFromFront(list *lst) {
	if(lst->length<=0) return 0;
	void *r=lst->head->obj;
	llist *oldHead=lst->head;
	lst->head=oldHead->next;
	lst->length--;
	free(oldHead);
	return r;
}



int bc_len(bytecode* bc) {
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

void expr_ensureLit(expression* exp) {
  if(exp->isLiteral) {
    exp->val.bcode = expr_toBc(exp);
    exp->isLiteral = false;
  }
}

void autocast(bool toFloat,expression *e) {

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
bytecode* expr_toBc(expression *exp) {
	if(exp->isLiteral) {
		bytecode* bc = malloc(sizeof(bytecode)*2);
		if(bc==NULL) internalCompileError("Out of memory");
		bc[0].code=BC_CPUSH;
		
		intfloat tmp;
		if(exp->isFloat)
		{
			tmp.f = exp->val.fl;
		}
		else 
		{
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

// TODO: use expr_free
void expr_free(expression* expr) {
  if(!expr->isLiteral) {
    assert(expr->val.bcode != (bytecode*)0);
      
    free(expr->val.bcode);
  }
  free(expr);
}
