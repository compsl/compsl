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
	
	
	//Tests
	
	{ //TEST 2 test push/pop
		bytecode code[] = 
		{ 
			{.code = 0x01, { {.a1 =0} } }, //push local at address 0
			{.code = 0x04, { {.a1 =1} }}, // pop into address 1
			{.code = 0x29}  // end NOTE MAKE SURE THIS IS THE CORRECT VALUE MAY CHANGE!!!
		};
		com->cubbys[0].code = code;
		com->vm = vm;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[1].v.i == com->vt.vars[0].v.i)
			printf("TEST 1 PASS!\n");
		else
			printf("TEST 1 FAIL!\n");
	}
	
	{// TEST 2
		//test push/pop/add/end
		bytecode code[] = 
		{ 
			{.code = 0x01, { {.a1 =0} } }, //push local at address 0
			{.code = 0x01, { {.a1 =1} } }, //push local at address 1
			{.code = 0x09}, // add
			{.code = 0x04, { {.a1 =0} }}, // pop into address 0
			{.code = 0x29}  // end NOTE MAKE SURE THIS IS THE CORRECT VALUE MAY CHANGE!!!
		};
		com->cubbys[0].code = code;
		com->vm = vm;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 3;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == 5)
			printf("TEST 2 PASS!\n");
		else
			printf("TEST 2 FAIL!\n");
	}
	
	{ //TEST 2
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
	return 0; //return 0 else make's unhappy
}
