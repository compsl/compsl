#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "vm.h"
#include "compart.h"
#include "var.h"
#include "intern/bytecode.h"
#include "intern/vm.h"
#include "compsl.h"

int main()
{
	VM *vm = createVM();
	compart *com = createComp();
	
	
	//Tests
	
	
	{// new block so code[] goes out of scope, this way we can re-use it's name in each test case
		//test push/pop/add/end
		bytecode code[] = 
		{ 
			{.code = 0x01, { {.a1 =0} } }, //push local at address 0
			{.code = 0x01, { {.a1 =1} } }, //push local at address 1
			{.code = 0x09}, // add
			{.code = 0x29}  // end NOTE MAKE SURE THIS IS THE CORRECT VALUE MAY CHANGE!!!
		};
		com->cubbys[0].code = code;
		com->vm = vm;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 3;
		
		runCubbyhole(com, 0);
	}
	//example case
	{
		bytecode code[] = 
		{ 
			{.code = 0x00, { {.a1 =0, .a2 = 0} }}, 
			{.code = 0x00, {.a = 0} },
			{.code = 0x00}
		};
	}
	
	//end tests
	
	destroyComp(com);
	destroyVM(vm);
	return 0; //return 0 else make's unhappy
}
