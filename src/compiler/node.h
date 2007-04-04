// $Id$

/*
    CompSL scripting language 
    Copyright (C) 2007  Thomas Jones & John Peberdy

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

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
