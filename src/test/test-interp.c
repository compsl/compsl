#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <float.h>
#include <math.h>

#include "intern/bytecode.h"
#include "compsl.h"
#include "intern/compartment.h"

int main()
{
	puts("Running interpreter tests");
	
	VM *vm = createVM();
	compart *com = createComp(vm);
	bool ret = 0;
	
	
	//Tests
	
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
