#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <float.h>
#include <math.h>

#include "../intern/bytecode.h"
#include "../extern/compsl.h"
#include "../intern/vars.h"


intfloat callTester(var *args); // function called to test CALL bytecode

int main()
{
	puts("Running interpreter tests");
	
	VM *vm = createVM();
	compart *com = createComp(vm);
	bool ret = 0;
	
	
	//Tests
	{ // PUSH,POP
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 1
			{.code = BC_END}  // end
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[1].v.i == com->vt.vars[0].v.i)
			printf("PUSH, POP: PASS!\n");
		else {
			printf("PUSH, POP: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // DUP
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_DUP},
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 1
			{.code = BC_POP, { {.a1 =2} }}, // pop into address 1
			{.code = BC_END}  // end
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		com->vt.vars[2].size =-1;
		com->vt.vars[2].v.i = 5;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[1].v.i == com->vt.vars[0].v.i
		    	&& com->vt.vars[1].v.i == com->vt.vars[2].v.i)
			printf("DUP: PASS!\n");
		else {
			printf("DUP: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // CPUSH
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =0} } }, //push constant at address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}  // end
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->cons[0].size =-1;
		com->cons[0].v.i = 3;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == com->cons[0].v.i)
			printf("CPUSH: PASS!\n");
		else {
			printf("CPUSH: FAIL!\n");
			ret = 1;
		}
	}
	
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
	
	{ // JMP 1 
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_PUSH, { {.a1 =2} } }, //push local at address 2
			{.code = BC_JMP, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 1;
		com->vt.vars[2].size =-1;
		com->vt.vars[2].v.i = 2;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 2)
			printf("JMP 1: PASS!\n");
		else 
		{
			if(com->vt.vars[0].v.i == 1)
				printf("JMP 1: FAIL! Didn't go far enough\n");
			else
				printf("JMP 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMP 2 (Backwards) this test is kinda dangerous, might infinite loop
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_PUSH, { {.a1 =2} } }, //push local at address 2
			{.code = BC_JMP, {.a = 4} }, // jump forward
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END},
			{.code = BC_JMP, {.sa = -2} }, // jump back 2
			{.code = BC_END} // just to be safe....
		};
		
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 1;
		com->vt.vars[2].size =-1;
		com->vt.vars[2].v.i = 2;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 2)
			printf("JMP 2: PASS!\n");
		else 
		{
			printf("JMP 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // JMN 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_PUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = -1;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("JMN 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("JMN 1: FAIL! Didn't go far enough\n");
			else
				printf("JMN 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMN 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_PUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[1].v.i == 1 && com->vt.vars[0].v.i == 2)
			printf("JMN 2: PASS!\n");
		else 
		{
			printf("JMN 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // JMZ 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_PUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_JMZ, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("JMZ 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("JMZ 1: FAIL! Didn't go far enough\n");
			else
				printf("JMZ 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMZ 2 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_PUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_JMZ, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 10;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[1].v.i == 1 && com->vt.vars[0].v.i == 2)
			printf("JMZ 2: PASS!\n");
		else 
		{
			printf("JMZ 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // EQ/JMN 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_EQ }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("EQ 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("EQ 1: FAIL! Didn't go far enough\n");
			else
				printf("EQ 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // EQ/JMN 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_EQ }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 2 && com->vt.vars[1].v.i == 1)
			printf("EQ 2: PASS!\n");
		else 
		{
			printf("EQ 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // LS/JMN 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_LS }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("LS 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("LS 1: FAIL! Didn't go far enough\n");
			else
				printf("LS 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMPL/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_LS }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 2 && com->vt.vars[1].v.i == 1)
			printf("LS 2: PASS!\n");
		else 
		{
			printf("LS 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // JMLE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_LE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("LE 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("LE 1: FAIL! Didn't go far enough\n");
			else
				printf("LE 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMPE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_LE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("LE 2: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("LE 2: FAIL! Didn't go far enough\n");
			else
				printf("LE 2: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMLE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_LE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 2 && com->vt.vars[1].v.i == 1)
			printf("LE 3: PASS!\n");
		else 
		{
			printf("LE 3: FAIL!\n");
			ret = 1;
		}
	}
	

	{ // JMPG/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_GR }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("GR 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("GR 1: FAIL! Didn't go far enough\n");
			else
				printf("GR 1: FAIL! When't too far\n");
			ret = 1;
		}
	}

	{ // JMPG/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_GR }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 2 && com->vt.vars[1].v.i == 1)
			printf("GR 2: PASS!\n");
		else 
		{
			printf("GR 2: FAIL!\n");
			ret = 1;
		}
	}
	
	
	{ // JMGE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_GE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("GE 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("GE 1: FAIL! Didn't go far enough\n");
			else
				printf("GE 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMGE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_GE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 1)
			printf("GE 2: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("GE 2: FAIL! Didn't go far enough\n");
			else
				printf("GE 2: FAIL! When't too far\n");
			ret = 1;
		}
	}

	{ // JMGE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_GE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 2;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 2 && com->vt.vars[1].v.i == 1)
			printf("GE 3: PASS!\n");
		else 
		{
			printf("GE 3: FAIL!\n");
			ret = 1;
		}
	}

	{ // EQ/JMN 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FEQ }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 1.0f)
			printf("FEQ 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.f == 1.0f)
				printf("FEQ 1: FAIL! Didn't go far enough\n");
			else
				printf("FEQ 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // EQ/JMZ2 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 2
			{.code = BC_CPUSH, { {.a1 =2} } }, //push const at address 1
			{.code = BC_FEQ }, // compare
			{.code = BC_JMZ, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f  = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f  = 2.0f;
		com->cons[2].size = -1;
		com->cons[2].v.f  = 0.0f;
		for(int i = 0; i < 10; i++)
			com->cons[2].v.f += 0.01f;
		com->cons[2].v.f *= 10.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 2.0f && com->vt.vars[1].v.f == 1.0f)
			printf("FEQ 2: PASS!\n");
		else 
		{
			printf("FEQ 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // EQ/JMZ3
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =2} } }, //push const at address 2
			{.code = BC_CPUSH, { {.a1 =3} } }, //push const at address 1
			{.code = BC_FEQ }, // compare
			{.code = BC_JMZ, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f  = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f  = 2.0f;
		com->cons[2].size = -1;
		com->cons[2].v.f  = ((float)INT_MAX)*((float)INT_MAX)/(1000.0f);
		com->cons[3].size = -1;
		com->cons[3].v.f  = 0.001f*((float)INT_MAX)*((float)INT_MAX);
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 2.0f && com->vt.vars[1].v.f == 1.0f)
			printf("FEQ 3: PASS!\n");
		else 
		{
			printf("FEQ 3: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // EQ/JMZ3
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =2} } }, //push const at address 2
			{.code = BC_CPUSH, { {.a1 =3} } }, //push const at address 1
			{.code = BC_FEQ }, // compare
			{.code = BC_JMZ, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f  = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f  = 2.0f;
		com->cons[2].size = -1;
		com->cons[2].v.f  = 5.0f*FLT_MIN-2.0f*FLT_MIN;
		com->cons[3].size = -1;
		com->cons[3].v.f  = 3.0f*FLT_MIN;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 2.0f && com->vt.vars[1].v.f == 1.0f)
			printf("FEQ 4: PASS!\n");
		else 
		{
			printf("FEQ 4: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // EQ/JMN 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 2
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_FEQ }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f  = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f  = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.f = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.f = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 2.0f && com->vt.vars[1].v.f == 1.0f)
			printf("FEQ 5: PASS!\n");
		else 
		{
			printf("FEQ 5: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // LS/JMN 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_FL }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 1.0f)
			printf("FL 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.f == 1.0f)
				printf("FL 1: FAIL! Didn't go far enough\n");
			else
				printf("FL 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMPL/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FL }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 2.0f && com->vt.vars[1].v.f == 1.0f)
			printf("FL 2: PASS!\n");
		else 
		{
			printf("FL 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // JMLE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_FLE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 1.0f)
			printf("FLE 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.f == 1.0f)
				printf("FLE 1: FAIL! Didn't go far enough\n");
			else
				printf("FLE 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMPE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FLE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 1.0f)
			printf("FLE 2: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.f == 1.0f)
				printf("FLE 2: FAIL! Didn't go far enough\n");
			else
				printf("FLE 2: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMLE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FLE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 2.0f && com->vt.vars[1].v.f == 1.0f)
			printf("FLE 3: PASS!\n");
		else 
		{
			printf("FLE 3: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // JMPG/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FGR }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 1.0f)
			printf("FGR 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.f == 1.0f)
				printf("FGR 1: FAIL! Didn't go far enough\n");
			else
				printf("FGR 1: FAIL! When't too far\n");
			ret = 1;
		}
	}

{ // JMPG/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FGR }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 2.0f && com->vt.vars[1].v.f == 1.0f)
			printf("FGR 2: PASS!\n");
		else 
		{
			printf("FGR 2: FAIL!\n");
			ret = 1;
		}
	}
	
	
	{ // JMGE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FGE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 1.0f)
			printf("FGE 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.f == 1.0f)
				printf("FGE 1: FAIL! Didn't go far enough\n");
			else
				printf("FGE 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMGE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FGE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 1.0f)
			printf("FGE 2: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.f == 1.0f)
				printf("FGE 2: FAIL! Didn't go far enough\n");
			else
				printf("FGE 2: FAIL! When't too far\n");
			ret = 1;
		}
	}

	{ // JMGE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_FGE }, // compare
			{.code = BC_JMN, {.a = 2} }, // jump forward, skip next instruction
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.f == 2.0f && com->vt.vars[1].v.f == 1.0f)
			printf("FGE 3: PASS!\n");
		else 
		{
			printf("FGE 3: FAIL!\n");
			ret = 1;
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
	
	{// CALL
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CALL, { {.a1= 0} }}, // call
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END} 
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 3;
		
		com->cons[0].size =-1;
		com->cons[0].v.i = 2;
		com->vt.vars[2].size = 2;
		com->vt.vars[2].p	= malloc(sizeof(intfloat)*2);
		com->vt.vars[2].p[0].i = 2;
		com->vt.vars[2].p[1].i = 4;
		
		com->vm->natives[0].func= &callTester;
		com->vm->natives[0].numParam = 3;
		com->vm->natives[0].params = malloc(3 * sizeof(var));
		com->vm->natives[0].paramFlags = malloc(3 * sizeof(uint8_t));
		com->vm->natives[0].paramFlags[0]=com->vm->natives[0].paramFlags[1]=0;
		
		com->vm->natives[0].paramFlags[2]=IS_ARRAY;
		com->vm->natives[0].isVoid = false;
		
		puts("CALL:");
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 11)
			printf("CALL: PASS!\n");
		else {
			ret = 1;
			printf("CALL: FAIL!\n");
		}
		free(com->vm->natives[0].params);
		free(com->vm->natives[0].paramFlags);
		free(com->vt.vars[2].p);
	}
	
	{//APUSH 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, {{.a1 =0}}},
			{.code = BC_APUSH, {{.a1 = 0}}}, 
			{.code = BC_CPUSH, {{.a1 =1}}}, 
			{.code = BC_APUSH, {{.a1 = 0}}}, 
			
			{.code = BC_POP, {{.a1 = 1}}},
			{.code = BC_POP, {{.a1 = 2}}},
			
			{.code = BC_END}
		};
		com->cubbys[0].code = code;
		
		
		com->cons[0].size =-1;
		com->cons[0].v.i = 0;
		com->cons[1].size =-1;
		com->cons[1].v.i = 1;
		
		com->vt.vars[0].size = 2;
		com->vt.vars[0].p	= malloc(sizeof(intfloat)*2);
		com->vt.vars[0].p[0].i = 2;
		com->vt.vars[0].p[1].i = 3;
		
		com->vt.vars[1].size = -1 ;
		com->vt.vars[2].size = -1 ;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[1].v.i == 3 && com->vt.vars[2].v.i == 2)
			printf("APUSH: PASS!\n");
		else {
			ret = 1;
			printf("APUSH: FAIL!\n\t%i %i",com->vt.vars[0].p[0].i,com->vt.vars[0].p[1].i);
		}
		free(com->vt.vars[0].p);
	}
	
	{//APOP
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, {{.a1 =0}}},
			{.code = BC_CPUSH, {{.a1 =0}}},	
			{.code = BC_CPUSH, {{.a1 =2}}},	
			{.code = BC_CPUSH, {{.a1 =1}}}, 
			{.code = BC_APOP, {{.a1 = 0}}},
			{.code = BC_APOP, {{.a1 = 0}}},
			
			{.code = BC_END}
		};
		com->cubbys[0].code = code;
		
		
		com->cons[0].size =-1;
		com->cons[0].v.i = 0;
		com->cons[1].size =-1;
		com->cons[1].v.i = 1;
		
		com->cons[2].size =-1;
		com->cons[2].v.i = 4;
		
		com->vt.vars[0].size = 2;
		com->vt.vars[0].p	= malloc(sizeof(intfloat)*2);
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].p[0].i == 0 && com->vt.vars[0].p[1].i == 4)
			printf("APOP: PASS!\n");
		else {
			ret = 1;
			printf("APOP: FAIL!\n\t%i %i",com->vt.vars[0].p[0].i,com->vt.vars[0].p[1].i);
		}
		
		free(com->vt.vars[0].p);
	}
	
//test builtins

	{
        bytecode code[] = 
        { 
            {.code = BC_CPUSH, { {.a1 =0, .a2 = 0} }}, 
            {.code = BC_CPUSH, { {.a1 =1, .a2 = 0} }},
            {.code = BC_ABS },
            {.code = BC_POP, {{.a1 = 1, .a2 = 0}}},
            {.code = BC_ABS },
            {.code = BC_POP, {{.a1 = 0, .a2 = 0}}},
            {.code = BC_END}
        };
        
        com->cons[0].size =-1;
        com->cons[0].v.i = -1;
        com->cons[1].size =-1;
        com->cons[1].v.i = 2;
        
        com->vt.vars[0].size =-1;
        com->vt.vars[0].v.i = 0;
        com->vt.vars[1].size =-1;
        com->vt.vars[1].v.i = 0;
        
        com->cubbys[0].code = code;
        
        runCubbyhole(com, 0);
        
        if(com->vt.vars[0].v.i == 1 &&  com->vt.vars[1].v.i == 2)
            printf("ABS: PASS!\n");
        else
            printf("ABS: FAIL!\n");
    }
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

intfloat callTester(var *args)
{
	intfloat v; 
	
	printf("\tCALL PART 1 PASS\n");
	if(args[2].size == 2)
		puts("\tCALL PART 2 PASS");
	
	v.i = args[0].v.i + args[1].v.i +args[2].p[0].i + args[2].p[1].i;
	
	return v; 
}
