// $Id: dumper.c 549 2007-10-04 03:52:45Z tomj $

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

/*
 * A file program to compile and dump the resulting bytecode
 * also used to experiment with new functions for messing with
 * compiler output (optimizers, more complex stackcheckers etc.)
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "compsl.h"

#include "intern/gen.h"
#include "intern/bytecode.h"
#include "intern/bcstrings.h"
#include "intern/compartment.h"

static void dmp(compart *com, int id)
{	
	bytecode *pc= (bytecode *)(com->cubbys[id].code); // init program counter
	
	while(pc->code != BC_END && pc->code != BC_HLT && pc->code != BC_DBG)
	{
		long int tractmp = ((long int)pc - (long int)(com->cubbys[id].code))/sizeof(bytecode);
		printf("%4ld: %5s  sa=%- 8d  a1= %u\n",
			tractmp,tractbl[pc->code],pc->sa,pc->a1);
		pc++;
	}
}

int main(int argc, char *argv[])
{
	int opt;
	bool dump, run, init;
	char *cubby = NULL;
	int compret; // return code of compiler
	int16_t cubbyid;
	
	dump = run = init = false;
	
	while((opt=getopt(argc,argv,"dric:")) != -1)
	{
		switch(opt) {
		case 'd':
			dump = true;
			break;
		case 'r':
			run = true;
			break;
		case 'i':
			init = true;
			break;
		case 'c':
			cubby = optarg;
			break;
		default:
			fprintf(stderr,"Usage: %s [-d] [-r] [-i] -c <cubby> <filename>\n",argv[1]);
			fprintf(stderr,"\t-d dump bytecode\n");
			fprintf(stderr,"\t-r run\n");
			fprintf(stderr,"\t-i run init\n");
			fprintf(stderr,"\t-c <name> run cubby\n");
			exit(EXIT_FAILURE);
		}
	}
	
	if(cubby == NULL || optind >= argc) { 
		fprintf(stderr,"Need a filename"); 
		exit(EXIT_FAILURE);
	}  
	
	VM *vm = createVM();
	if(vm == NULL) { fprintf(stderr,"Error initializing compsl"); exit(1);}
	
	addPrintLibToVm(vm);
	addDebugLibToVm(vm);
	
	compart *com = createComp(vm);
	
	if(com == NULL) {
		fprintf(stderr,"Error initializing compsl");
		exit(EXIT_FAILURE);
	}
	
	compret = fileCompile(argv[optind], com);
	
	if(compret) {
		fprintf(stderr,"Compile Error"); 
		exit(EXIT_FAILURE);
	}
	
	if(init) {
		cubbyid = getCubbyID(com,"init");
		runCubbyhole(com, cubbyid);
	}
	
	cubbyid = getCubbyID(com, cubby);
	
	if(dump) dmp(com,cubbyid);
	if(run) runCubbyhole(com, cubbyid); 
	
	destroyComp(com);	
	destroyVM(vm);
}
