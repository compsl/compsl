#include "extern/vm.h"
#include "extern/compart.h"
#include "extern/compslerr.h"
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

#ifndef __GNUC__
#error "gotta have gcc!"
#endif

#define VM_FLOAT_EPSILON 0.001f

void runCubbyhole(compart *com, int id)
{
static void *jmptbl[] = 
{
	&&NOOP,
//stack operations with local vars
	&&PUSH,
 	&&APUSH,
 	&&CPUSH,
 	&&POP, 
 	&&APOP,
 	&&DPOP,
 	&&SWAP,
 // native function call
 	&&CALL, 
 // integer operations
 	&&ADD, 
 	&&SUB, 
 	&&MUL, 
 	&&DIV, 
 	&&CMP,
 // float operations 
 	&&FADD, 
 	&&FSUB, 
 	&&FMUL,
 	&&FDIV, 	
 	&&FCMP,
 // jumps 
 	&&JMP, 	
 	&&JMPL, 	
 	&&JMPE, 	
 	&&JMPG, 	
 	&&JMLE, 	
 	&&JMNE,
 	&&JMGE,
 //type conversion
 	&&FLIN, 
 	&&INFL,
 //mod
 	&&MOD, 
 	&&FMOD,
 //global vars
 	&&GPSH,
 	&&GAPS,
 	&&GPOP,
 	&&GAPP,
 //boolean 
 	&&AND,
 	&&OR,
 	&&NOT,
 //bitwise
 	&&BAND,
 	&&BOR,
 	&&BXOR,
 	&&BNOT,
 //bit shifting
 	&&SFTL,
 	&&SFTR,
 //misc
 	&&PYES,
 	&&NONO,
 	&&END,
 	&&HLT,
 	&&DBG

 };
	intfloat stack[VM_STACK_SIZE];
	intfloat *sp = stack; // stack pointer
	intfloat tmp;
	
	var *lvs = com->vt.vars;
	var *lcs = com->cons;
	var *gvs = com->vm->vt.vars;
	
	nativeFN *natives = com->vm->natives;
	
	bytecode *pc= (bytecode *)(com->cubbys[id].code) - 1; // init program counter
	
	bool less, equal, greater; // comparison flags
	less = equal = greater = false;
	
	TOP: 
		pc++;
		goto *jmptbl[pc->code]; // highly unreabable, but it gets the bytecode,  and jumps to the correct instruction
	NOOP:
		goto TOP;
	PUSH:
		*sp = lvs[pc->a1].v;
		sp++;
		goto TOP;
 	APUSH://TODO: test this
 		if(lvs[pc->a1].size > (sp - 1)->i || (sp - 1)->i < 0)
 		{
 			*(sp - 1) = lvs[pc->a1].p[(sp - 1)->i];
 		}
 		else
 		{
 			fprintf(stderr, 
 				"ERROR: Index out of bounds! \n\tvar: %i %s\n\tsize: %i\n\tindex: %i", 
 				pc->a1, com->vt.symbols[pc->a1].name, lvs[pc->a1].size, (sp - 1)->i);
 			(sp - 1)->i = 0;//hmmm what to do? index out of bounds!
#ifdef DEBUG
				goto DBG;
#else
 				goto HLT;
#endif
 			
 		}
 		goto TOP;
 	CPUSH:
 		*sp = lcs[pc->a1].v;
 		sp++;
		goto TOP;
 	POP:
 		sp--;
 		lvs[pc->a1].v = *sp;
		goto TOP;
 	APOP:
 		//TODO: test this
 		sp -= 2;
 		if(lvs[pc->a1].size > sp->i || sp->i < 0)
 		{
 			lvs[pc->a1].p[sp->i] = *(sp + 1);
 		}
 		else
 		{
 			fprintf(stderr, 
 				"ERROR: Index out of bounds! \n\tvar: %i %s\n\tsize: %i\n\tindex: %i", 
 				pc->a1, com->vt.symbols[pc->a1].name, lvs[pc->a1].size, sp->i);
#ifdef DEBUG
 				goto DBG;
#else
 				goto HLT;
#endif
 		}
 		goto TOP;
 	DPOP:
 		sp--;
 		goto TOP;
 	SWAP://TODO: should this be an addressed thing? (ie: can swap an arbitrary pair of stack elements)
 		tmp = *(sp - 1);
 		*(sp - 1) = *(sp - 2);
 		*(sp - 2) = tmp;
 		goto TOP;
 	CALL: 
 	//TODO: testme
 		for(int i=0; i<natives[pc->a1].numParam; i++)
 		{
 			if(natives[pc->a1].paramFlags[i] & IS_ARRAY)
 				if(natives[pc->a1].paramFlags[i] & IS_GLBL)
	 				natives[pc->a1].params[i] = lvs[(--sp)->i];
	 			else
	 				natives[pc->a1].params[i] = gvs[(--sp)->i];
 			else
 				natives[pc->a1].params[i].v = *(--sp);
 		}
 		*sp = (natives[pc->a1].func)(natives[pc->a1].params);
 		sp++;
 		
 		goto TOP;
 	ADD:
 		sp--;
 		(sp - 1)->i = (sp - 1)->i + sp->i;
 		goto TOP;
 	SUB:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i - sp->i;
 		goto TOP;
 	MUL:
	 	sp--;
 		(sp - 1)->i = (sp - 1)->i * sp->i;
 		goto TOP;
 	DIV:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i / sp->i;
 		goto TOP;
 	CMP:
 	// TODO: see if this can be done faster
 		sp-=2;
 		tmp.i = sp->i - (sp+1)->i;
 		less = tmp.i < 0;
 		equal = !tmp.i;
 		greater = !less;
 		goto TOP;
 	FADD:
	 	sp--;
 		(sp - 1)->f = (sp - 1)->f + sp->f;
 		goto TOP;
 	FSUB:
	 	sp--;
 		(sp - 1)->f = (sp - 1)->f - sp->f;
 		goto TOP; 
 	FMUL:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f * sp->f;
 		goto TOP; 
 	FDIV:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f / sp->f;
 		goto TOP; 
 	FCMP:
	 	sp-=2;
 		tmp.f = sp->f - (sp + 1)->f;
 		less = tmp.f < 0;
 		//equal = fabsf(tmp.f) <VM_FLOAT_EPSILON;
 		//equal = fabsf( 1.0f - (sp->f)/((sp + 1)->f)) < VM_FLOAT_EPSILON; // compensate for float inaccuracys
 		equal = fabsf( tmp.f/(sp->f) ) < VM_FLOAT_EPSILON;
 		greater = !less;
 		goto TOP;
 	JMP:
 		pc += pc->sa - 1; // compensate for pc++ at top
 		goto TOP;
 	JMPL:
 		if(less) pc += pc->sa - 1;// compensate for pc++ at top
 		goto TOP;	
 	JMPE:
	 	if(equal) pc += pc->sa - 1;// compensate for pc++ at top
 		goto TOP; 	
 	JMPG:
 		if(greater) pc += pc->sa - 1;// compensate for pc++ at top
 		goto TOP;
 	JMLE:
 		if(less || equal) pc += pc->sa - 1;// compensate for pc++ at top
 		goto TOP;
 	JMNE:
 		if(!equal) pc += pc->sa - 1;// compensate for pc++ at top
 		goto TOP; 	
 	JMGE:
 		if(greater || equal) pc += pc->sa - 1;// compensate for pc++ at top
 		goto TOP;
 	FLIN: 
 		sp->i = (int)(sp->f);
 		goto TOP;
 	INFL:
 		sp->f = (float)(sp->i);
 		goto TOP;
 	MOD:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i % sp->i;
 		goto TOP;
 	FMOD:
 		sp--;	
 		//(sp - 1)->f = (sp - 1)->f - sp->f * truncf((sp - 1)->f / sp->f); // a ? ((int)(a / b)) * b
#ifndef DEBUG
 		(sp - 1)->f = fmodf((sp - 1)->f, sp->f);
#endif
 		//(sp - 1)->f = 42;
 		//TODO: uncomment above
 		goto TOP;
 	GPSH:
 		*sp = gvs[pc->a1].v;
		sp++;
		goto TOP;
 	GAPS:
 		//TODO: writeme
 		goto UNIMP;
 	GPOP:
 		sp--;
 		gvs[pc->a1].v = *sp;
		goto TOP;
 	GAPP:
 //TODO: writeme
 		goto UNIMP;
 		
 //begin of boolean + bitwise opers
 
 //boolean 
 	AND:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i && sp->i;
 		goto TOP;
 	OR:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i || sp->i;
 		goto TOP;
 	NOT:
 		(sp - 1)->i = !((sp - 1)->i);
 		goto TOP;
 //bitwise
 	BAND:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i & sp->i;
 		goto TOP;
 	BOR:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i | sp->i;
 		goto TOP;
 	BXOR:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i ^ sp->i;
 		goto TOP;
 	BNOT: 
 		(sp - 1)->i = ~((sp - 1)->i);
 		goto TOP;
 //bit shift
 	SFTL:
 		sp--;
 		(sp - 1)->i = (sp - 1)->i << sp->i;
 		goto TOP;
 	SFTR:
 		sp--;
 		(sp - 1)->i = (sp - 1)->i >> sp->i;
 		goto TOP;
 
 	PYES:
 		if(pc->a1) {puts("YES");}
 		else { puts("NO"); }
 		goto TOP;
 	NONO:
		puts("OMG WTF NO!!!");	
 	END:
 	HLT:
 		return;
 	DBG: // dump some state info
#ifdef DEBUG
 	{
 		var *t;
 		intfloat *st;
 		
 		fprintf(stderr, "Program Counter: %X", (unsigned int)((bytecode *)(com->cubbys[id].code) - pc));
 		
 		fprintf(stderr, "Compare Flags (e,l,g): %s %s %s\n",
 			(equal?"true":"false"),
 			(less?"true":"false"),
 			(greater?"true":"false"));
 		fprintf(stderr, "Stack Pointer: %X\n", (unsigned int)(sp - stack)); // what does this do? index of sp?
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
 		goto TOP;
 	UNIMP:
 	
 	panic("unimplemented instruction");
}
