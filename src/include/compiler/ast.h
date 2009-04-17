/*
 * ast.h
 *
 *  Created on: Apr 14, 2009
 *      Author: john
 */

#ifndef AST_H_
#define AST_H_

#include <stdbool.h>
#include <compsl.h>
#include <stdlib.h>


// vtable definitions
typedef struct _ast_con_vt ast_con_vt;
typedef struct _ast_node_vt ast_node_vt;


// Node types
typedef struct _ast_node ast_node;
typedef struct _ast_con ast_con;
typedef struct _ast_binop ast_binop;
typedef struct _ast_numliteral ast_numliteral;


struct _ast_node_vt {
	void (*print)(ast_node*);
};

struct _ast_con_vt {
	ast_node_vt super;
	int (*size)(ast_node*);
};

// Node type definitions

struct _ast_node {
	ast_node_vt *vt;
	char data[0];
};

struct _ast_con {
	ast_node super;
	ast_node *children[0];
};

struct _ast_binop {
	ast_con super;
	ast_node *children[2];
	int op;
};

struct _ast_numliteral {
	ast_node super;
	bool bInt;
	intfloat payload;
};

ast_node *ast_create_numliteral_int(int32_t num);
ast_node *ast_create_numliteral_float(float num);
ast_node *ast_create_binop(int op, ast_node *a, ast_node *b);

#endif /* AST_H_ */
