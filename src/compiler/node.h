#include <stdbool.h>

#ifndef NODE_H_
#define NODE_H_

#define CSL_FLOAT true;
#define CSL_INT false;

typedef struct _node_t
{
	char* foo;
	int f;
} node;


typedef struct _expression_t {
	bool isLiteral;
	bool isFloat;	
	union { int in; float fl; } val;
	
} expression;




#endif 
