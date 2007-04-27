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

#include "extern/compsl.h"

#include "intern/panic.h"
#include "intern/vars.h"
#include "intern/bytecode.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

#ifndef __GNUC__
#error "gotta have gcc!"
#endif

#ifdef DEBUG
#define STOPEXEC goto DBG
#else
#define STOPEXEC goto HLT
#endif


#define COMPSL_NEXT_INSTRUCTION goto *(&&NOOP + jmptbl[(++pc)->code])
#ifdef _COMPSL_TRACE
#define COMPSL_END_INST goto TOP
#else
#define COMPSL_END_INST COMPSL_NEXT_INSTRUCTION
#endif


#define VM_FLOAT_EPSILON FLT_EPSILON

double genrand_real1(void);

#if defined DEBUG || defined _COMPSL_TRACE
static const char *tractbl[] = 
{
	"NOOP",
//stack operations with local vars
	"PUSH",
	"PSHT",
 	"APUSH",
 	"CPUSH",
 	"GPSH",
 	"GAPS",
 	"POP",
 	"POPT", 
 	"APOP",
 	"DPOP",
 	"GPOP",
 	"GAPP",
 	"DUP",
 // native function call
 	"CALL", 
 // integer operations
 	"ADD", 
 	"SUB", 
 	"MUL", 
 	"DIV", 
 	"LE",
 	"LS",
	"EQ",
	"NE",
 	"GR",
 	"GE",
 //mod
 	"MOD", 
 	"FMOD",
 //boolean 
 	"AND",
 	"OR",
 	"NOT",
 //bitwise
 	"BAND",
 	"BOR",
 	"BXOR",
 	"BNOT",
 //bit shifting
 	"SFTL",
 	"SFTR",
 // float operations 
 	"FADD", 
 	"FSUB", 
 	"FMUL",
 	"FDIV", 
 	"FLE",
 	"FLS",
	"FEQ",
	"FNE",
 	"FGR",
 	"FGE",
 // jumps 
 	"JMP",
 	"JMZ",
 	"JMN",
 //type conversion
 	"FLIN", 
 	"INFL",
 	"SAVE",
 	"STO",
 	"GSTO",
 	"INC",
	"DEC",
 	"FINC",
	"FDEC",
//builtins
	"ABS",
	"ABSF",
	"SIN",
	"COS",
	"TAN",
	"ASIN",
	"ACOS",
	"ATAN",
	"SQRT",
	"LN",
	"FLOOR",
	"CEIL",
	"RAND",
	"ATAN2",
	"POW",
	"MIN",
	"MAX",
	"MINF",
	"MAXF",
	"HYPOT",
//misc
 	"PYES",
 	"NONO",
 	"END",
 	"HLT",
 	"DBG"

 };
#endif

#ifdef DEBUG
void dumpBytecode(compart *com, int id)
{
	//var *lvs = com->vt.vars;
	//var *lcs = com->cons;
	//var *gvs = com->vm->vt.vars;
	
	//nativeFN *natives = com->vm->natives;
	
	bytecode *pc= (bytecode *)(com->cubbys[id].code); // init program counter
	
	while(pc->code != BC_END && pc->code != BC_HLT && pc->code != BC_DBG)
	{
		long int tractmp = ((long int)pc - (long int)(com->cubbys[id].code))/sizeof(bytecode);
		printf("%4ld: %5s  sa=%- 8d  a1= %u\n",
			tractmp,tractbl[pc->code],pc->sa,pc->a1);
		pc++;
	}
}

void dumpBytecode2(compart *com, bytecode *code)
{	
	bytecode *pc= code;
	while(pc->code != BC_END && pc->code != BC_HLT && pc->code != BC_DBG && pc->code != BC_NONO)
	{
		long int tractmp = ((long int)pc - (long int)code)/sizeof(bytecode);
		printf("%4ld: %5s  sa=%- 8d  a1= %u\n",
			tractmp,tractbl[pc->code],pc->sa,pc->a1);
		pc++;
	}
}
#endif

COMPSL_EXPORT void runCubbyhole(compart *com, int id)
{
static const int jmptbl[] = 
{
	&&NOOP - &&NOOP,
//stack operations
	&&PUSH - &&NOOP,
	&&PSHT - &&NOOP,
 	&&APUSH - &&NOOP,
 	&&CPUSH - &&NOOP,
 	&&GPSH - &&NOOP,
 	&&GAPS - &&NOOP,
 	&&POP - &&NOOP,
 	&&POPT - &&NOOP,
 	&&APOP - &&NOOP,
 	&&DPOP - &&NOOP,
 	&&GPOP - &&NOOP,
 	&&GAPP - &&NOOP,
 	&&DUP - &&NOOP,
 // native function call
 	&&CALL - &&NOOP,
 // integer operations
 	&&ADD - &&NOOP,
 	&&SUB - &&NOOP,
 	&&MUL - &&NOOP,
 	&&DIV - &&NOOP,
 //compares
 	&&LE - &&NOOP,
 	&&LS - &&NOOP,
	&&EQ - &&NOOP,
	&&NE - &&NOOP,
 	&&GR - &&NOOP,
 	&&GE - &&NOOP,
 //mod
 	&&MOD - &&NOOP,
 	&&FMOD - &&NOOP,
 //boolean 
 	&&AND - &&NOOP,
 	&&OR - &&NOOP,
 	&&NOT - &&NOOP,
 //bitwise
 	&&BAND - &&NOOP,
 	&&BOR - &&NOOP,
 	&&BXOR - &&NOOP,
 	&&BNOT - &&NOOP,
 //bit shifting
 	&&SFTL - &&NOOP,
 	&&SFTR - &&NOOP,
 // float operations 
 	&&FADD - &&NOOP,
 	&&FSUB - &&NOOP,
 	&&FMUL - &&NOOP,
 	&&FDIV - &&NOOP,
 //float compares
 	&&FLE - &&NOOP,
 	&&FLS - &&NOOP,
	&&FEQ - &&NOOP,
	&&FNE - &&NOOP,
 	&&FGR - &&NOOP,
 	&&FGE - &&NOOP,
 // jumps 
 	&&JMP - &&NOOP,
 	&&JMZ - &&NOOP,
 	&&JMN - &&NOOP,
 //type conversion
 	&&FLIN - &&NOOP,
 	&&INFL - &&NOOP,
 	&&SAVE - &&NOOP,
 	&&STO - &&NOOP,
 	&&GSTO - &&NOOP,
 	&&INC - &&NOOP,
 	&&DEC - &&NOOP,
 	&&FINC - &&NOOP,
 	&&FDEC - &&NOOP,
//builtins
	&&ABS - &&NOOP,
	&&ABSF - &&NOOP,
	&&SIN - &&NOOP,
	&&COS - &&NOOP,
	&&TAN - &&NOOP,
	&&ASIN - &&NOOP,
	&&ACOS - &&NOOP,
	&&ATAN - &&NOOP,
	&&SQRT - &&NOOP,
	&&LN - &&NOOP,
	&&FLOOR - &&NOOP,
	&&CEIL - &&NOOP,
	&&RAND - &&NOOP,
	&&ATAN2 - &&NOOP,
	&&POW - &&NOOP,
	&&MIN - &&NOOP,
	&&MAX - &&NOOP,
	&&MINF - &&NOOP,
	&&MAXF - &&NOOP,
	&&HYPOT - &&NOOP,
//misc
 	&&PYES - &&NOOP,
 	&&NONO - &&NOOP,
 	&&END - &&NOOP,
 	&&HLT - &&NOOP,
 	&&DBG - &&NOOP

 };
 
 	#ifdef _COMPSL_TRACE
 		long int tractmp,sppos;
 	#endif
 
	intfloat stack[VM_STACK_SIZE];
	intfloat temp; // temp register
	register intfloat *sp = stack; // stack pointer
	
	var *lvs = com->vt.vars;
	var *lcs = com->cons;
	var *gvs = com->vm->vt.vars;
	
	nativeFN *natives = com->vm->natives;
	
	register bytecode *pc= (bytecode *)(com->cubbys[id].code) - 1; // init program counter
	
	TOP: 
	NOOP:
	
		#ifdef _COMPSL_TRACE
			
			tractmp = ((long int)(pc+1) - (long int)(com->cubbys[id].code))/sizeof(bytecode);
			sppos =  ((long int)sp - (long int)stack)/sizeof(intfloat);
			printf("%4ld: %s\tsa=%- 8d\ta1=%- 8d\tsp=%ld\n",
				tractmp,tractbl[(pc+1)->code],(pc+1)->sa,(pc+1)->a1,sppos);
		#endif
		
		COMPSL_NEXT_INSTRUCTION; // highly unreabable, but it gets the bytecode,  and jumps to the correct instruction
	
	PUSH:
		*sp = lvs[pc->a1].v;
		sp++;
		COMPSL_END_INST;
 	APUSH:
 		if(__builtin_expect(lvs[pc->a1].size > (sp - 1)->i && (sp - 1)->i >= 0,1))
 		{
 			*(sp - 1) = lvs[pc->a1].p[(sp - 1)->i];
 			COMPSL_END_INST;
 		}
 		else
 		{
 			fprintf(stderr, 
 				"ERROR: Index out of bounds! \n\tvar: %i %s\n\tsize: %i\n\tindex: %i", 
 				pc->a1, com->vt.symbols[pc->a1].name, lvs[pc->a1].size, (sp - 1)->i);
 			(sp - 1)->i = 0;//hmmm what to do? index out of bounds!

			STOPEXEC; 			
 		}
 	CPUSH:
 		*sp = lcs[pc->a1].v;
 		sp++;
		COMPSL_END_INST;

    POP:
 		sp--;
 		lvs[pc->a1].v = *sp;
		COMPSL_END_INST;
 	APOP:
 		sp -=2;
 		if(__builtin_expect(lvs[pc->a1].size > (sp+1)->i && (sp+1)->i >= 0,1))
 		{
 			lvs[pc->a1].p[(sp+1)->i] = *(sp);
 			COMPSL_END_INST;
 		}
 		else {
 			fprintf(stderr, 
 				"ERROR: Index out of bounds! (Local array) \n\tvar: %s (id=%i)\n\tsize: %i\n\tindex: %i\n", 
 				com->vt.symbols[pc->a1].name, pc->a1, lvs[pc->a1].size, (sp+1)->i);

			STOPEXEC;
 		}
 	DPOP:
 		sp--;
 		COMPSL_END_INST;
 	//SWAP:
 	//	tmp = *(sp - 1);
 	//	*(sp - 1) = *(sp - 2);
 	//	*(sp - 2) = tmp;
 	//	COMPSL_END_INST;
 	DUP:
 		*sp = *(sp - 1);
 		sp++;
 		COMPSL_END_INST;
 	SAVE:
 		temp = *(sp - 1);
 		COMPSL_END_INST;
 	STO:
 		lvs[pc->a1].v = *(sp-1);
		COMPSL_END_INST;
 	GSTO:
 		gvs[pc->a1].v = *(sp-1);
		COMPSL_END_INST;
 	PSHT:
 		*sp = temp;
 		sp++;
 		COMPSL_END_INST;
 	POPT:
 		sp--;
 		temp = *sp;
 		COMPSL_END_INST;
 	CALL: 
 		for(int i=0; i<natives[pc->a1].numParam; i++)
 		{
 			if(natives[pc->a1].paramFlags[i] & IS_ARRAY)
 				if(natives[pc->a1].paramFlags[i] & IS_GLBL)
	 				natives[pc->a1].params[i] = gvs[(--sp)->i];
	 			else
	 				natives[pc->a1].params[i] = lvs[(--sp)->i];
 			else
 				natives[pc->a1].params[i].v = *(--sp);
 		}
 		
		if(!natives[pc->a1].isVoid)
		  *sp = (natives[pc->a1].func)(natives[pc->a1].params);
 		else 
		  (natives[pc->a1].func)(natives[pc->a1].params);
		sp++;
		
 		COMPSL_END_INST;
 		
 	ADD:
 		sp--;
 		(sp - 1)->i = (sp - 1)->i + sp->i;
 		COMPSL_END_INST;
 	INC:
 		(sp - 1)->i++;
 		COMPSL_END_INST;
 	FINC:
 		(sp - 1)->f++;
 		COMPSL_END_INST;
 	SUB:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i - sp->i;
 		COMPSL_END_INST;
 	DEC:
 		(sp - 1)->i--;
 		COMPSL_END_INST;
 	FDEC:
 		(sp - 1)->f--;
 		COMPSL_END_INST;
 	MUL:
	 	sp--;
 		(sp - 1)->i = (sp - 1)->i * sp->i;
 		COMPSL_END_INST;
 	DIV:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i / sp->i;
 		COMPSL_END_INST;
	LE:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i <= sp->i;
 		COMPSL_END_INST;
 	LS:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i < sp->i;
 		COMPSL_END_INST;
	EQ:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i == sp->i;
 		COMPSL_END_INST;
	NE:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i != sp->i;
 		COMPSL_END_INST;
 	GR:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i > sp->i;
 		COMPSL_END_INST;
 	GE:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i >= sp->i;
 		COMPSL_END_INST;
 		
 	FADD:
	 	sp--;
 		(sp - 1)->f = (sp - 1)->f + sp->f;
 		COMPSL_END_INST;
 	FSUB:
	 	sp--;
 		(sp - 1)->f = (sp - 1)->f - sp->f;
 		COMPSL_END_INST; 
 	FMUL:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f * sp->f;
 		COMPSL_END_INST; 
 	FDIV:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f / sp->f;
 		COMPSL_END_INST; 
 	FLE:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f <= sp->f;
 		COMPSL_END_INST; 
 	FLS:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f < sp->f;
 		COMPSL_END_INST;
	FEQ:
 		sp--;
 		(sp - 1)->f = fdimf(sp->f, (sp - 1)->f) < VM_FLOAT_EPSILON;
 		COMPSL_END_INST;
	FNE:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f != sp->f;
 		COMPSL_END_INST;
 	FGR:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f > sp->f;
 		COMPSL_END_INST;
 	FGE:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f >= sp->f;
 		COMPSL_END_INST;
 	JMP:
 		pc += pc->sa - 1; // compensate for pc++ at top
 		COMPSL_END_INST;
 	JMZ:
 		if(!((--sp)->i)) pc += pc->sa - 1; // compensate for pc++ at top
 		COMPSL_END_INST;
 	JMN:
 		if((--sp)->i) pc += pc->sa - 1; // compensate for pc++ at top
 		COMPSL_END_INST;
 	FLIN: 
 		(sp-1)->i = (int)((sp-1)->f);
 		COMPSL_END_INST;
 	INFL:
 		(sp-1)->f = (float)((sp-1)->i);
 		COMPSL_END_INST;
 	MOD:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i % sp->i;
 		COMPSL_END_INST;
 	FMOD:
 		sp--;	
 		(sp - 1)->f = fmodf((sp - 1)->f, sp->f);
 		COMPSL_END_INST;
 	GPSH:
 		*sp = gvs[pc->a1].v;
		sp++;
		COMPSL_END_INST;
 	GAPS:
		if(__builtin_expect(gvs[pc->a1].size > (sp - 1)->i && (sp - 1)->i >= 0,1))
 		{
 			*(sp - 1) = gvs[pc->a1].p[(sp - 1)->i];
 			COMPSL_END_INST;
 		}
 		else
 		{
 			fprintf(stderr, 
 				"ERROR: Index out of bounds! (global array)\n\tvar: %s (id=%i)\n\tsize: %i\n\tindex: %i", 
 				com->vt.symbols[pc->a1].name, pc->a1, gvs[pc->a1].size, (sp - 1)->i);
 			(sp - 1)->i = 0;//hmmm what to do? index out of bounds!

			STOPEXEC;
 			
 		}
 		
 	GPOP:
 		sp--;
 		gvs[pc->a1].v = *sp;
		COMPSL_END_INST;
		
 	GAPP:
 		sp -=2;
 		if(__builtin_expect(gvs[pc->a1].size > (sp+1)->i && (sp+1)->i >= 0,1))
 		{
 			gvs[pc->a1].p[(sp+1)->i] = *(sp);
 			COMPSL_END_INST;
 		}
 		else
 		{
 			fprintf(stderr, 
 				"ERROR: Index out of bounds! \n\tvar: %s (id=%i)\n\tsize: %i\n\tindex: %i", 
 				com->vt.symbols[pc->a1].name, pc->a1, gvs[pc->a1].size, (sp+1)->i);

			STOPEXEC;
 		}

 //boolean 
 	AND:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i && sp->i;
 		COMPSL_END_INST;
 	OR:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i || sp->i;
 		COMPSL_END_INST;
 	NOT:
 		(sp - 1)->i = !((sp - 1)->i);
 		COMPSL_END_INST;
 //bitwise
 	BAND:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i & sp->i;
 		COMPSL_END_INST;
 	BOR:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i | sp->i;
 		COMPSL_END_INST;
 	BXOR:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i ^ sp->i;
 		COMPSL_END_INST;
 	BNOT: 
 		(sp - 1)->i = ~((sp - 1)->i);
 		COMPSL_END_INST;
 //bit shift
 	SFTL:
 		sp--;
 		(sp - 1)->i = (sp - 1)->i << sp->i;
 		COMPSL_END_INST;
 	SFTR:
 		sp--;
 		(sp - 1)->i = (sp - 1)->i >> sp->i;
 		COMPSL_END_INST;
	
//bultins
	
	ABS:
		(sp - 1)->i = abs((sp - 1)->i);
 		COMPSL_END_INST;
	
	ABSF:
		(sp - 1)->f = fabsf((sp - 1)->f);
		COMPSL_END_INST;
	SIN:
		(sp - 1)->f = sinf((sp - 1)->f);
		COMPSL_END_INST;
	COS:
		(sp - 1)->f = cosf((sp - 1)->f);
		COMPSL_END_INST;
	TAN:
		(sp - 1)->f = tanf((sp - 1)->f);
		COMPSL_END_INST;
	ASIN:
		(sp - 1)->f = asinf((sp - 1)->f);
		COMPSL_END_INST;
	ACOS:
		(sp - 1)->f = acosf((sp - 1)->f);
		COMPSL_END_INST;
	ATAN:
		(sp - 1)->f = atanf((sp - 1)->f);
		COMPSL_END_INST;
	SQRT:
		(sp - 1)->f = sqrtf((sp - 1)->f);
		COMPSL_END_INST;
	LN:
		(sp - 1)->f = logf((sp - 1)->f);
		COMPSL_END_INST;
	FLOOR:
		(sp - 1)->f = floorf((sp - 1)->f);
		COMPSL_END_INST;
	CEIL:
		(sp - 1)->f = ceilf((sp - 1)->f);
		COMPSL_END_INST;
	RAND:
		sp->f = (float)(genrand_real1());
		sp++;
		COMPSL_END_INST;
	ATAN2:
		sp--;
		(sp - 1)->f = atan2f(sp->f, (sp - 1)->f);
		COMPSL_END_INST;
	POW:
		sp--;
		(sp - 1)->f = powf(sp->f, (sp - 1)->f);
		COMPSL_END_INST;
	MIN:
		sp--;
		(sp - 1)->i = (sp->i < (sp - 1)->i)?sp->i:(sp - 1)->i;
		COMPSL_END_INST;
	MAX:
		sp--;
		(sp - 1)->i = (sp->i > (sp - 1)->i)?sp->i:(sp - 1)->i;
		COMPSL_END_INST;
	MINF:
		sp--;
		(sp - 1)->f = fminf(sp->f, (sp - 1)->f);
		COMPSL_END_INST;
	MAXF:
		sp--;
		(sp - 1)->f = fmaxf(sp->f, (sp - 1)->f);
		COMPSL_END_INST;
	HYPOT:
		sp--;
		(sp - 1)->f = hypotf(sp->f, (sp - 1)->f);
		COMPSL_END_INST;
//misc	
 	PYES:
 		if(pc->a1) {puts("YES");}
 		else { puts("NO"); }
 		COMPSL_END_INST;
 		
 	NONO:
		puts("OMG WTF NO!!!");	
	//icky things	
// 	UNIMP:
// 	
// 	panic("unimplemented instruction");
 	
 	DBG: // dump some state info
#ifdef DEBUG
 	{
 		var *t;
 		intfloat *st;
 		
 		fprintf(stderr, "Program Counter: %lX", ((long int)pc - (long int)(com->cubbys[id].code))/sizeof(bytecode));
 		
 		fprintf(stderr, "Stack Pointer: %lX\n", ((long int)sp - (long int)stack)/sizeof(intfloat)); // what does this do? index of sp?
 		fprintf(stderr, "Stack: \n");
 		st = stack;
 		for(unsigned int i = 0; i < VM_STACK_SIZE; i++, st++)
	 		fprintf(stderr,"\t%X   %i %f\n", i, (int)(st->i), (st->f)); 

 		fprintf(stderr, "Local variables:\n");
 		t = lvs;
 		for(unsigned int i = 0 ; i < COMPART_MAX_VARS; i++, t++)
 			if(t->size <= 0) 
 				fprintf(stderr, "\t%X:   %i %f\n", i, (int)(t->v.i), t->v.f);
 		
 		fprintf(stderr, "Global Vars:\n");
 		t = gvs;
 		for(unsigned int i = 0 ; i < VM_MAX_GVARS; i++, t++)
 			if(t->size <= 0) 
 				fprintf(stderr, "\t%X:   %i %f\n",i, (int)(t->v.i), t->v.f);
 	}
#endif
	END:
 	HLT:
 	
 	
 	return;
}
