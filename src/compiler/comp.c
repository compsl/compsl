#include <stdio.h>
#include "node.h"

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

bytecode* expr_toBc(expression *exp) {
	if(exp->isLiteral) {
		bytecode* bc = malloc(sizeof(bytecode)*(2));
		bc[0].code=BC_PUSH;
		//TODO: push onto stack
	}
	else {
		return exp->val.bcode;
	}
}