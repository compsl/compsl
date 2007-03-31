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
#include <limits.h>
#include <float.h>
#include <time.h>

#ifndef __GNUC__
#error "gotta have gcc!"
#endif

#define VM_FLOAT_EPSILON FLT_EPSILON

void init_genrand(unsigned long s);
double genrand_real1(void);

#if defined DEBUG || defined _COMPSL_TRACE
static const char *tractbl[] = 
{
	"NOOP",
//stack operations with local vars
	"PUSH",
 	"APUSH",
 	"CPUSH",
 	"GPSH",
 	"GAPS",
 	"POP", 
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
		printf("%4ld: %s\tsa=%d\ta1=%d\n",
			tractmp,tractbl[pc->code],pc->sa,pc->a1);
		pc++;
	}
}
#endif

__attribute__((flatten,regparam (2))) void runCubbyhole(compart *com, int id)
{
static const int jmptbl[] = 
{
	&&NOOP - &&NOOP,
//stack operations
	&&PUSH - &&NOOP,
 	&&APUSH - &&NOOP,
 	&&CPUSH - &&NOOP,
 	&&GPSH - &&NOOP,
 	&&GAPS - &&NOOP,
 	&&POP - &&NOOP,
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
 
	static intfloat stack[VM_STACK_SIZE];
	intfloat *sp = stack; // stack pointer
	
	var *lvs = com->vt.vars;
	var *lcs = com->cons;
	var *gvs = com->vm->vt.vars;
	
	nativeFN *natives = com->vm->natives;
	
	bytecode *pc= (bytecode *)(com->cubbys[id].code) - 1; // init program counter
	
	//bool less, equal, greater; // comparison flags
	//less = equal = greater = false;
	
	//seed the rng
	init_genrand(time(0));
	
	TOP: 
		#ifdef _COMPSL_TRACE
			
			tractmp = ((long int)(pc+1) - (long int)(com->cubbys[id].code))/sizeof(bytecode);
			sppos =  ((long int)sp - (long int)stack)/sizeof(intfloat);
			printf("%4ld: %s\tsa=%d\ta1=%d\tsp=%ld\n",
				tractmp,tractbl[(pc+1)->code],(pc+1)->sa,(pc+1)->a1,sppos);
		#endif
		goto *(&&NOOP + jmptbl[(++pc)->code]); // highly unreabable, but it gets the bytecode,  and jumps to the correct instruction
	NOOP:
		goto TOP;
	PUSH:
		*sp = lvs[pc->a1].v;
		sp++;
		goto TOP;
 	APUSH:
 		if(lvs[pc->a1].size > (sp - 1)->i && (sp - 1)->i >= 0)
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
 		sp -=2;
 		if(lvs[pc->a1].size > (sp)->i && (sp)->i >= 0)
 		{
 			lvs[pc->a1].p[(sp)->i] = *(sp+1);
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
 	//SWAP://TODO: should this be an addressed thing? (ie: can swap an arbitrary pair of stack elements)
 	//	tmp = *(sp - 1);
 	///	*(sp - 1) = *(sp - 2);
 	//	*(sp - 2) = tmp;
 	//	goto TOP;
 	DUP:
 		*sp = *(sp - 1);
 		sp++;
 		goto TOP;
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
	LE:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i <= sp->i;
 		goto TOP;
 	LS:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i < sp->i;
 		goto TOP;
	EQ:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i == sp->i;
 		goto TOP;
	NE:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i != sp->i;
 		goto TOP;
 	GR:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i > sp->i;
 		goto TOP;
 	GE:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i >= sp->i;
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
 	FLE:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f <= sp->f;
 		goto TOP; 
 	FLS:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f < sp->f;
 		goto TOP;
	FEQ:
 		sp--;
 		(sp - 1)->f = fdimf(sp->f, (sp - 1)->f) < VM_FLOAT_EPSILON;
 		goto TOP;
	FNE:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f != sp->f;
 		goto TOP;
 	FGR:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f > sp->f;
 		goto TOP;
 	FGE:
 		sp--;
 		(sp - 1)->f = (sp - 1)->f >= sp->f;
 		goto TOP;
 	JMP:
 		pc += pc->sa - 1; // compensate for pc++ at top
 		goto TOP;
 	JMZ:
 		if(!((--sp)->i)) pc += pc->sa - 1; // compensate for pc++ at top
 		goto TOP;
 	JMN:
 		if((--sp)->i) pc += pc->sa - 1; // compensate for pc++ at top
 		goto TOP;
 	FLIN: 
 		(sp-1)->i = (int)((sp-1)->f);
 		goto TOP;
 	INFL:
 		(sp-1)->f = (float)((sp-1)->i);
 		goto TOP;
 	MOD:
 		sp--;	
 		(sp - 1)->i = (sp - 1)->i % sp->i;
 		goto TOP;
 	FMOD:
 		sp--;	
 		(sp - 1)->f = fmodf((sp - 1)->f, sp->f);
 		goto TOP;
 	GPSH:
 		*sp = gvs[pc->a1].v;
		sp++;
		goto TOP;
 	GAPS:
		if(gvs[pc->a1].size > (sp - 1)->i && (sp - 1)->i >= 0)
 		{
 			*(sp - 1) = gvs[pc->a1].p[(sp - 1)->i];
 		}
 		else
 		{
 			fprintf(stderr, 
 				"ERROR: Index out of bounds! \n\tvar: %i %s\n\tsize: %i\n\tindex: %i", 
 				pc->a1, com->vt.symbols[pc->a1].name, gvs[pc->a1].size, (sp - 1)->i);
 			(sp - 1)->i = 0;//hmmm what to do? index out of bounds!
#ifdef DEBUG
				goto DBG;
#else
 				goto HLT;
#endif
 			
 		}
 		goto TOP;
 	GPOP:
 		sp--;
 		gvs[pc->a1].v = *sp;
		goto TOP;
 	GAPP:
 		//TODO: test this
 		sp -=2;
 		if(gvs[pc->a1].size > (sp)->i && (sp)->i >= 0)
 		{
 			gvs[pc->a1].p[(sp)->i] = *(sp+1);
 		}
 		else
 		{
 			fprintf(stderr, 
 				"ERROR: Index out of bounds! \n\tvar: %i %s\n\tsize: %i\n\tindex: %i", 
 				pc->a1, com->vt.symbols[pc->a1].name, gvs[pc->a1].size, sp->i);
#ifdef DEBUG
 				goto DBG;
#else
 				goto HLT;
#endif
 		}
 		goto TOP;
 		
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
	
//bultins
	
	ABS:
		(sp - 1)->i = abs((sp - 1)->i);
 		goto TOP;
	
	ABSF:
		(sp - 1)->f = fabsf((sp - 1)->f);
		goto TOP;
	SIN:
		(sp - 1)->f = sinf((sp - 1)->f);
		goto TOP;
	COS:
		(sp - 1)->f = cosf((sp - 1)->f);
		goto TOP;
	TAN:
		(sp - 1)->f = tanf((sp - 1)->f);
		goto TOP;
	ASIN:
		(sp - 1)->f = asinf((sp - 1)->f);
		goto TOP;
	ACOS:
		(sp - 1)->f = acosf((sp - 1)->f);
		goto TOP;
	ATAN:
		(sp - 1)->f = atanf((sp - 1)->f);
		goto TOP;
	SQRT:
		(sp - 1)->f = sqrtf((sp - 1)->f);
		goto TOP;
	LN:
		(sp - 1)->f = logf((sp - 1)->f);
		goto TOP;
	FLOOR:
		(sp - 1)->f = floorf((sp - 1)->f);
		goto TOP;
	CEIL:
		(sp - 1)->f = ceilf((sp - 1)->f);
		goto TOP;
	RAND:
		sp->f = (float)(genrand_real1());
		sp++;
		goto TOP;
	ATAN2:
		sp--;
		(sp - 1)->f = atan2f(sp->f, (sp - 1)->f);
		goto TOP;
	POW:
		sp--;
		(sp - 1)->f = powf(sp->f, (sp - 1)->f);
		goto TOP;
	MIN:
		sp--;
		(sp - 1)->i = (sp->i < (sp - 1)->i)?sp->i:(sp - 1)->i;
		goto TOP;
	MAX:
		sp--;
		(sp - 1)->i = (sp->i > (sp - 1)->i)?sp->i:(sp - 1)->i;
		goto TOP;
	MINF:
		sp--;
		(sp - 1)->f = fminf(sp->f, (sp - 1)->f);
		goto TOP;
	MAXF:
		sp--;
		(sp - 1)->f = fmaxf(sp->f, (sp - 1)->f);
		goto TOP;
	HYPOT:
		sp--;
		(sp - 1)->f = hypotf(sp->f, (sp - 1)->f);
		goto TOP;
//misc	
 	PYES:
 		if(pc->a1) {puts("YES");}
 		else { puts("NO"); }
 		goto TOP;
 	NONO:
		puts("OMG WTF NO!!!");	
	//icky things	
	
 	END:
 	HLT:
 		return;
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
 		goto TOP;
// 	UNIMP:
// 	
// 	panic("unimplemented instruction");
}
