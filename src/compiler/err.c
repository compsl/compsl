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

#include "interncomp.h"
#include <stdlib.h>
#include <stdio.h>


void compileError(const char *str) {
  fflush(stderr);
  fflush(stdout);
  fprintf(stderr,"%s:%i: error: %s\n",csl_name,lineNo,str);
}

 void compileWarning(const char *str) {
  fflush(stderr);
  fflush(stdout);
  fprintf(stderr,"%s:%i: warning: %s\n",csl_name,lineNo,str);
 }

char foo[1024];

void internalCompileError(const char* str) {
  DPRINTF("internal Compile error\n");
  fprintf(stderr,"%s:%i: error: INTERNAL ERROR: %s\n",csl_name,lineNo,str);	
  abort();
}
