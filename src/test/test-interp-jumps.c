#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <float.h>
#include <math.h>

#include "intern/bytecode.h"
#include "compsl.h"
#include "intern/vars.h"
#include "intern/compartment.h"

intfloat callTester(var *args); // function called to test CALL bytecode

int main()
{
	puts("Running interpreter jump tests");
	
	VM *vm = createVM();
	compart *com = createComp(vm);
	bool ret = 0;
	
	
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
