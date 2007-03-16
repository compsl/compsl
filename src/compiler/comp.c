#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "../extern/compart.h"
#include "../intern/compartment.h"
#include "../intern/vars.h"

compart *ccompart;


void internalCompileError(const char* str) {
	sprintf(sprt,"Internal error: %s",str);
	compileError(sprt);	
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
	
void list_free(list *lst){
	//TODO: memory leak here
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


void autocast(bool toFloat,expression *e) {
	if(e->isLiteral) {
		if( toFloat && !e->isFloat) {
			e->val.fl = (float)e->val.in;
			e->isFloat=true;
		}
		else if(!toFloat && e->isFloat) {
			e->val.in = (int)e->val.fl;
			e->isFloat=false;
		} 
	}
	else {
		internalCompileError("Casting not fully implemented yet");
	}
}

/*
 * Note: doesnt free anything
 */
bytecode* expr_toBc(expression *exp) {
	if(exp->isLiteral) {
		bytecode* bc = malloc(sizeof(bytecode)*2);
		if(bc==0) internalCompileError("Out of memory");
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
