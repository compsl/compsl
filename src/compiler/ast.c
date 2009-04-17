/*
 * ast.c
 *
 *  Created on: Apr 14, 2009
 *      Author: john
 */

#include <stdio.h>
#include "compiler/ast.h"

// AST operation implementations

static void numliteral_print(ast_node *rthiz) {
	ast_numliteral *thiz = (ast_numliteral*) rthiz;

	if (thiz->bInt) {
		printf("%d", thiz->payload.i);
	} else {
		printf("%f", thiz->payload.f);
	}
}

static void astcon_print(ast_node *rthiz) {
	ast_con *thiz = (ast_con*) rthiz;
	ast_node **chld = &thiz->children[0];
	int n = ((ast_con_vt*) thiz->super.vt)->size(rthiz);

	printf("<");

	while (n--) {
		(*chld)->vt->print(*chld);
		chld++;

		if (n) {
			printf(", ");
		}
	}

	printf(">");
}

int astbinop_size(ast_node *rthiz) {
	return 2;
}

// Vtable definitions

ast_node_vt astvt_numliteral = { .print = numliteral_print };
ast_con_vt astvt_binop = { .super = {.print = astcon_print},
		.size = astbinop_size };

// Helper function

static ast_node *nodemalloc(ast_node_vt *vt, size_t sz) {
	ast_node *nod = (ast_node*) malloc(sz);

	nod->vt = vt;

	return nod;
}

// API functions

ast_node*
ast_create_numliteral_int(int32_t num) {
	ast_numliteral *nod = (ast_numliteral*) nodemalloc(&astvt_numliteral,
			sizeof(ast_numliteral));

	nod->bInt = true;
	nod->payload.i = num;

	return (ast_node*) nod;
}

ast_node*
ast_create_numliteral_float(float num) {
	ast_numliteral *nod = (ast_numliteral*) nodemalloc(&astvt_numliteral,
			sizeof(ast_numliteral));

	nod->bInt = false;
	nod->payload.f = num;

	return (ast_node*) nod;
}

ast_node*
ast_create_binop(int op, ast_node *a, ast_node *b) {
	ast_binop *nod = (ast_binop*)nodemalloc((ast_node_vt*)(&astvt_binop), sizeof(ast_binop));

	nod->children[0] = a;
	nod->children[1] = b;
	nod->op = op;

	return (ast_node*)nod;
}
