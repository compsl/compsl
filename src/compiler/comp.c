#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "node.h"
#include "../intern/vars.h"


list* list_new(void) {
	list *lst = malloc(sizeof(list));
	if(lst == NULL) internalCompileError("Out of Memory");
	lst->length=0;
	lst->head=NULL;
	return lst;
}

// gets the ind'th element in lst
llist* list_getNode(list * lst, int ind) {
  if(ind>=lst->length || ind<0)
    return 0;

  llist *root = lst->head;
  while(root!=NULL && ind > 0) {
    root=root->next;
    ind--;
  }
  if(root!=NULL) 
    return root;
  assert(false);
}

void* list_get(list * lst, int ind) {
  return list_getNode(lst,ind)->obj;
}

// Note: contents aren't freed
void llist_free(llist *lst) {
  if(lst==NULL) return;
  llist *next = lst->next;
  free(lst); lst = NULL;
  llist_free(next);
}	

// Note, contents aren't freed
void list_free(list *lst){
  llist_free(lst->head);
  free(lst); lst = NULL;
}

void list_addToFront(list *lst, void *newOb) {
	llist *newL = malloc(sizeof(llist));
	if(newL == NULL) internalCompileError("Out of Memory");
	newL->obj=newOb;
	newL->next=lst->head;
	lst->head=newL;
	lst->length++;
}
void list_addToBack(list *lst, void *newOb) {
  assert(lst!=NULL);
  assert(newOb!=NULL);
  llist *newL = malloc(sizeof(llist));
  newL->obj = newOb;
  newL->next = NULL;
  assert(lst->length>=0);
  if(lst->length==0) 
    lst->head = newL;
  else
    list_getNode(lst, lst->length-1)->next = newL;
  
  lst->length++;
}


void* list_popFromFront(list *lst) {
	if(lst->length<=0) return NULL;
	void *r=lst->head->obj;
	llist *oldHead=lst->head;
	lst->head=oldHead->next;
	lst->length--;
	free(oldHead);
	return r;
}
