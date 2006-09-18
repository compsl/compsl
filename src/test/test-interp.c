#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../extern/var.h"
#include "../intern/bytecode.h"
#include "../extern/compsl.h"


intfloat callTester(var *args); // function called to test CALL bytecode

int main()
{
	VM *vm = createVM();
	compart *com = createComp(vm);
	bool ret = 0;
	
	
	//Tests
	printf("Running interpreter tests\n");
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
		
		if(com->vt.vars[0].v.f == 2.5f)
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
		
		if(com->vt.vars[0].v.f == 1.5f)
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
		
		if(com->vt.vars[0].v.f == 1.0f)
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
		
		if(com->vt.vars[0].v.f == 4.0f)
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
	
	{ // JMPE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CMP }, // compare
			{.code = BC_JMPE, {.a = 2} }, // jump forward, skip next instruction
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
			printf("JMPE 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("JMPE 1: FAIL! Didn't go far enough\n");
			else
				printf("JMPE 1: FAIL! When't too far\n");
			ret = 1;
		}
	}
	
	{ // JMPE/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CMP }, // compare
			{.code = BC_JMPE, {.a = 2} }, // jump forward, skip next instruction
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
			printf("JMPE 2: PASS!\n");
		else 
		{
			printf("JMPE 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // JMPL/CMP 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CMP }, // compare
			{.code = BC_JMPL, {.a = 2} }, // jump forward, skip next instruction
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
			printf("JMPL 1: PASS!\n");
		else 
		{
			if(com->vt.vars[1].v.i == 1)
				printf("JMPL 1: FAIL! Didn't go far enough\n");
			else
				printf("JMPL 1: FAIL! When't too far\n");
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
			{.code = BC_CMP }, // compare
			{.code = BC_JMPL, {.a = 2} }, // jump forward, skip next instruction
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
			printf("JMPL 2: PASS!\n");
		else 
		{
			printf("JMPL 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{// CALL
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_CALL, { {.a1= 0} }}, // call
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_END} 
		};
		com->cubbys[0].code = code;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 3;
		
		com->vm->natives[0].func= &callTester;
		com->vm->natives[0].numParam = 2;
		com->vm->natives[0].params = malloc(2 * sizeof(var));
		com->vm->natives[0].paramFlags = malloc(2 * sizeof(uint8_t));
		com->vm->natives[0].paramFlags[0]=com->vm->natives[0].paramFlags[1]=0;
		com->vm->natives[0].isVoid = false;
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 5)
			printf("CALL: PASS!\n");
		else {
			ret = 1;
			printf("CALL: FAIL!\n");
		}
	}
	
	//TODO: test APUSH/APOP
	
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
	}*/
	
	//end tests
	
	destroyComp(com);
	destroyVM(vm);
	return ret; //return 0 else make's unhappy
}

intfloat callTester(var *args)
{
	intfloat v; 
	v.i = args[0].v.i + args[1].v.i;
	printf("CALL PART 1 PASS\n\targs: %i %i\n",args[0].v.i, args[1].v.i);
	return v; 
}
