#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <float.h>
#include <math.h>

#include "intern/bytecode.h"
#include "compsl.h"
#include "intern/compartment.h"

/*******************************************************
 * Tester for arithmatic operations
 *******************************************************/

int main()
{
	puts("Running interpreter arithmetic tests");
	
	VM *vm = createVM();
	compart *com = createComp(vm);
	bool ret = 0;
	
	/************************ Begin Tests *************************************/

	{// ADD
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_ADD}, // add
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END} 
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 3;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 5)
			printf("ADD: PASS!\n");
		else {
			ret = 1;
			printf("ADD: FAIL!\n");
		}
	}
	
	{ //SUB
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_SUB}, // sub
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}  // end NOTE MAKE SURE THIS IS THE CORRECT VALUE MAY CHANGE!!!
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 3;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == -1)
			printf("SUB: PASS!\n");
		else { 
			printf("SUB: FAIL!\n");
			ret = 1;
		}
	}
	
	{ //MUL
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_MUL}, // multiply
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}  // end NOTE MAKE SURE THIS IS THE CORRECT VALUE MAY CHANGE!!!
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 3;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 6)
			printf("MUL: PASS!\n");
		else { 
			printf("MUL: FAIL!\n");
			ret = 1;
		}
	}
	
	{ //DIV
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_DIV}, // divide
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}  // end NOTE MAKE SURE THIS IS THE CORRECT VALUE MAY CHANGE!!!
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 16;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 2;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 8)
			printf("DIV: PASS!\n");
		else { 
			printf("DIV: FAIL!\n");
			ret = 1;
		}
	}
	
	//TODO: fix comparisons in these tests
	
	{// FADD
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_FADD}, // add
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END} 
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 2.0f;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0.5f;
		
		runCubbyhole(com, 0);
		
		if(fabs(com->vt.vars[0].v.f - 2.5f) < FLT_EPSILON)
			printf("FADD: PASS!\n");
		else {
			ret = 1;
			printf("FADD: FAIL!\n");
		}
	}
	
	{// FSUB
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_FSUB}, // subtract
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END} 
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 2.0f;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0.5f;
		
		runCubbyhole(com, 0);
		
		if(fabs(com->vt.vars[0].v.f - 1.5f) < FLT_EPSILON)
			printf("FSUB: PASS!\n");
		else {
			ret = 1;
			printf("FSUB: FAIL!\n");
		}
	}
	
	{// FMUL
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_FMUL}, // multiply
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END} 
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 2.0f;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0.5f;
		
		runCubbyhole(com, 0);
		
		if(fabs(com->vt.vars[0].v.f - 1.0f) < FLT_EPSILON)
			printf("FMUL: PASS!\n");
		else {
			ret = 1;
			printf("FMUL: FAIL!\n");
		}
	}
	
	{// FDIV
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_FDIV}, // divide
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END} 
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 2.0f;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0.5f;
		
		runCubbyhole(com, 0);
		
		if(fabs(com->vt.vars[0].v.f - 4.0f) < FLT_EPSILON)
			printf("FDIV: PASS!\n");
		else {
			ret = 1;
			printf("FDIV: FAIL!\n");
		}
	}
	
	{ //FLIN/INFL
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =0} }}, 
			{.code = BC_FLIN},
			{.code = BC_POP , { {.a1 =0} }},
			{.code = BC_PUSH, { {.a1 =0} }}, 
			{.code = BC_INFL},
			{.code = BC_POP , { {.a1 =1} }},
			{.code = BC_END}
		};
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.5f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		puts("FLIN/INFL:");
		
		if(com->vt.vars[0].v.i == 1)
			puts("\tFLIN: PASS");
		else
			printf("\tFLIN: FAIL!\n\t\t%i\n", com->vt.vars[0].v.i);
			
		if(abs(com->vt.vars[1].v.f - (float)1) < 0.0001)
			puts("\tINFL: PASS");
		else
			printf("\tINFL: FAIL!\n\t\t%f\n", com->vt.vars[1].v.f);
	}
	
	/*TODO: 
	BC_MOD,		//modulus of top 2 stack b = pop; a = pop; push a % b;
	BC_FMOD,	//same as MOD only for float
	BC_AND,		//boolean and of top two stack elements
	BC_OR,		//boolean or of top two stack elements
	BC_NOT, 	//boolean not of top stack element
	BC_BAND,	//bitwise and of top two stack elements
	BC_BOR,		//bitwise or of top two stack elements
	BC_BXOR,	//bitwise xor of top two stack elements
	BC_BNOT,	//bitwise not of top stack element
	BC_SFTL,	//shift left, top of stack is shift, next is value
	BC_SFTR,	//shift right, top of stack is shift, next is value
	BC_INC,		//increment top of stack
	BC_DEC,		//decrement top of stack
	BC_FINC,	//increment top of stack
	BC_FDEC,	//decrement top of stack
	*/
	
	/*
	//example case
	{ // new block so code[] goes out of scope, this way we can re-use it's name in each test case
		bytecode code[] = 
		{ 
			{.code = BC_NOOP, { {.a1 =0, .a2 = 0} }}, 
			{.code = BC_NOOP, {.a = 0} },
			{.code = BC_NOOP},
			{.code = BC_END}
		};
		com->cubbys[0].code = code;
		
		runCubbyhole(com, 0);
	}*/
	
	//end tests
	
	com->cubbys[0].code = NULL;
	
	destroyComp(com);
	destroyVM(vm);
	return ret; //return 0 else make's unhappy
}
