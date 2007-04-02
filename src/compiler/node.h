#include <stdbool.h>
#include "../intern/bytecode.h"
#include "../extern/compsl.h"

#include "../intern/debug.h"

#ifndef NODE_H_
#define NODE_H_

#define DPRINTF(...) COMP_DPRINTF(__VA_ARGS__) 


#define CSL_FLOAT 2;
#define CSL_INT 1;


typedef struct llist {
	void *obj;
	void *next; //TODO: make llist type
} llist;

typedef struct _list_t {
	int length;
	llist *head;
} list;


list* list_new(void);
void* list_get(list *, int);
void list_free(list *);
void llist_free(llist *);
void list_addToFront(list *, void *);
void list_addToBack(list *, void *);
void* list_popFromFront(list *);

extern char *sprt;


#endif 
