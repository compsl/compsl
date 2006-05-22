#include <stdio.h>
#include "node.h"

compart *ccompart;

// gets the ind'th element in lst
void* list_get(list * lst, int ind) {
	llist *root = lst->head;
	while(root!=0 && ind-- > 0) {
		root=root->next;
	}
	if(root!=0) { 
		if(ind>=lst->length) {puts("ERROR in list_get"); }
		return root->obj;
	}
	return 0;
}
	
void list_free(list * lst){
	//TODO: memory leak here
	free(lst);
}


/*
 * Note: doesnt free anything
 */
bytecode* expr_toBc(expression *exp) {
	if(exp->isLiteral) {
		bytecode* bc = malloc(sizeof(bytecode)*(2));
		
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
		return exp->val.bcode;
	}
}