#include "extern/vm.h"
#include "extern/compart.h"
#include "extern/compslerr.h"
#include "extern/compsl.h"

#include "intern/panic.h"
#include "intern/vm.h"
#include "intern/vars.h"
#include "intern/bytecode.h"

#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

#define VM_FLOAT_EPSILON 0.1f

void runCubbyhole(compart *com, int id)
{
static void *jmptbl[] = 
{
	&&NOOP,
	&&PUSH,
 	&&APUSH,
 	&&CPUSH,
 	&&POP, 
 	&&APOP,
 	&&DPOP,
 	&&SWAP,
 	&&CALL, 
 	&&ADD, 
 	&&SUB, 
 	&&MUL, 
 	&&DIV, 
 	&&CMP, 
 	&&FADD, 
 	&&FSUB, 
 	&&FMUL,
 	&&FDIV, 	
 	&&FCMP, 
 	&&JMP, 	
 	&&JMPL, 	
 	&&JMPE, 	
 	&&JMPG, 	
 	&&JMLE, 	
 	&&JMNE, 	
 	&&JMGE,
 	&&FLIN, 
 	&&INFL,
 	&&MOD, 
 	&&FMOD,
 	&&GPSH,
 	&&GAPS,
 	&&GPOP,
 	&&GAPP,
 	&&END,
 	&&HLT,
 	&&DBG
 };
	intfloat stack[VM_STACK_SIZE];
	intfloat *sp = stack; // stack pointer
	intfloat tmp;
	
	var *lvs = com->vt.vars;
	var *lcs = com->ct.vars;
	var *gvs = com->vm->vt.vars;
	
	bytecode *pc= (bytecode *)(com->cubbys[id].code);
	
	bool less, equal, greater;
	
	goto START;
	
	TOP:
		pc++;
	START:
		goto *jmptbl[pc->code]; // highly unreabable, but it gets the bytecode,  and jumps to the correct instruction
	NOOP:
		goto TOP;
	PUSH:
		*sp = lvs[pc->a1].v;
		sp++;
		goto TOP;
 	APUSH:
 		//TODO: writeme
 		goto UNIMP;
 	CPUSH:
 		*sp = lcs[pc->a1].v;
 		sp++;
		goto TOP;
 	POP:
 		lvs[pc->a1].v = *sp;
 		sp--;
		goto TOP;
 	APOP:
 		//TODO: writeme
 		goto UNIMP;
 	DPOP:
 		sp--;
 		goto TOP;
 	SWAP:
 	//TODO: writeme
 		goto UNIMP;
 	CALL: 
 	//TODO: writeme
 		goto UNIMP;
 	ADD:
 		sp-=2;
 		sp->i = sp->i + (sp+1)->i;
 		goto TOP;
 	SUB:
 		sp-=2;	
 		sp->i = sp->i - (sp+1)->i;
 		goto TOP;
 	MUL:
	 	sp-=2;
 		sp->i = sp->i * (sp+1)->i;
 		goto TOP;
 	DIV:
 		sp-=2;	
 		sp->i = sp->i / (sp+1)->i;
 		goto TOP;
 	CMP:
 	// TODO: see if this can be done faster
 		sp-=2;
 		tmp.i = sp->i - (sp+1)->i;
 		less = tmp.i < 0;
 		equal = !tmp.i;
 		greater = tmp.i > 0;
 		goto TOP;
 	FADD:
	 	sp-=2;
 		sp->f = sp->f + (sp+1)->f;
 		goto TOP;
 	FSUB:
	 	sp-=2;
 		sp->f = sp->f - (sp+1)->f;
 		goto TOP; 
 	FMUL:
 		sp-=2;
 		sp->f = sp->f * (sp+1)->f;
 		goto TOP; 
 	FDIV:
 		sp-=2;
 		sp->f = sp->f / (sp+1)->f;
 		goto TOP; 
 	FCMP:
	 	sp-=2;
 		tmp.f = sp->f - (sp+1)->f;
 		less = tmp.f < 0;
 		equal = fabs(tmp.f) < VM_FLOAT_EPSILON;
 		greater = tmp.f > 0;
 		goto TOP;
 	JMP:
 		pc += pc->sa;
 		goto START;
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
 		sp-=2;	
 		sp->i = sp->i % (sp+1)->i;
 		goto TOP;
 	FMOD:
 		sp-=2;	
 		sp->f = sp->f - (sp+1)->f * (int)(sp->f / (sp+1)->f); // a ? ((int)(a / b)) * b
 		goto TOP;
 	GPSH:
 		*sp = gvs[pc->a1].v;
		sp++;
		goto TOP;
 	GAPS:
 		//TODO: writeme
 		goto UNIMP;
 	GPOP:
 		gvs[pc->a1].v = *sp;
		sp--;
		goto TOP;
 	GAPP:
 //TODO: writeme
 		goto UNIMP;
 	HLT:
 	
 	END:
 		return;
 	DBG: // dump some state info
 		
 		goto TOP;
 	
 	UNIMP:
 	
 	panic("unimplemented instruction");
}
