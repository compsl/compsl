#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../extern/var.h"
#include "../intern/bytecode.h"
#include "../intern/vm.h"
#include "../extern/compsl.h"

int main()
{
	VM *vm = createVM();
	compart *com = createComp();
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
		com->vm = vm;
		
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
		com->vm = vm;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->ct.vars[0].size =-1;
		com->ct.vars[0].v.i = 3;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == com->ct.vars[0].v.i)
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
		com->vm = vm;
		
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
		com->vm = vm;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 3;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == -1)
			printf("SUB: PASS!\n");
		else { 
			printf("SUB: FAIL!%i\n");
			ret = 1;
		}
	}
	/*
	//example case
	{ // new block so code[] goes out of scope, this way we can re-use it's name in each test case
		bytecode code[] = 
		{ 
			{.code = 0x00, { {.a1 =0, .a2 = 0} }}, 
			{.code = 0x00, {.a = 0} },
			{.code = 0x00}
		};
	}*/
	
	//end tests
	
	destroyComp(com);
	destroyVM(vm);
	return ret; //return 0 else make's unhappy
}
