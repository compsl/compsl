#include <stdbool.h>
#include "../intern/bytecode.h"
#include "../extern/vm.h"
#include "../extern/compart.h"

#ifndef NODE_H_
#define NODE_H_

#define CSL_FLOAT 2;
#define CSL_INT 1;

VM* cvm;
compart *ccompart;

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

typedef struct llist {
	void *obj;
	void *next; //TODO: make llist type
} llist;

typedef struct _list_t {
	int length;
	llist *head;
} list;

void* list_get(list *, int);
void list_free(list *);


#endif 
