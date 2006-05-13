#include <stdbool.h>
#include "../intern/bytecode.h"

#ifndef NODE_H_
#define NODE_H_

#define CSL_FLOAT 2;
#define CSL_INT 1;

typedef struct _node_t
{
	char* foo;
	int f;
} node;


typedef struct _expression_t {
	bool isLiteral;
	bool isFloat;	
	union { 
		int in; 
		float fl; 
		bytecode* bcode; 
	} val;
} expression;




#endif 
