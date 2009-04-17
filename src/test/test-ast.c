/*
 * test-ash.c
 *
 *  Created on: Apr 14, 2009
 *      Author: john
 */

#include "compiler/ast.h"
#include <stdio.h>

int main()
{
	printf("Running AST tests - kinda\n");

	ast_node *l1 = ast_create_numliteral_int(44);
	ast_node *l2 = ast_create_numliteral_float(44.0);
	ast_node *op1 = ast_create_binop(1, l1, l2);

	printf("Testing print\n");
	l1->vt->print(l1);
	printf("\n");
	op1->vt->print(op1);



}
