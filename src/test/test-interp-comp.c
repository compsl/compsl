#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <float.h>
#include <math.h>

#include "../intern/bytecode.h"
#include "../extern/compsl.h"
#include "../intern/vars.h"


/*******************************************************
 * Tester for the most basic instructions (stack operations)
 *******************************************************/

int main()
{
	puts("Running interpreter comparison tests");
	
	VM *vm = createVM();
	compart *com = createComp(vm);
	bool ret = 0;
	
	/************************ Begin Tests *************************************/
	{ // EQ
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_EQ }, // compare
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_EQ }, // compare
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 1
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
		
		if(com->vt.vars[0].v.i && !com->vt.vars[1].v.i)
			printf("EQ: PASS!\n");
		else 
		{
			if(!com->vt.vars[0].v.i) printf("EQ 1: FAIL!\n");
			if( com->vt.vars[1].v.i) printf("EQ 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // LS 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_LS }, // compare
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			//test2
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_LS }, // compare
			{.code = BC_POP, { {.a1 =1} }}, // pop into 1
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
		
		if(com->vt.vars[0].v.i && !com->vt.vars[1].v.i)
			printf("LS: PASS!\n");
		else 
		{
			if(!com->vt.vars[0].v.i) printf("LS 1: FAIL!\n");
			if( com->vt.vars[1].v.i) printf("LS 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // LE
		bytecode code[] = 
		{ 
			//test 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_LE }, // compare
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			//test2
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_LE }, // compare
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			//test3
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_LE }, // compare
			{.code = BC_POP, { {.a1 =2} }}, // pop into address 0
			//end
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
		com->vt.vars[2].size =-1;
		com->vt.vars[2].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i && com->vt.vars[1].v.i && !com->vt.vars[2].v.i)
			printf("LE: PASS!\n");
		else 
		{
			if(!com->vt.vars[0].v.i) printf("LE 1: FAIL!\n");
			if(!com->vt.vars[1].v.i) printf("LE 2: FAIL!\n");
			if( com->vt.vars[2].v.i) printf("LE 3: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // GR 
		bytecode code[] = 
		{ 
			//test1
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_GR }, // compare
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			//test2
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_GR }, // compare
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			//end
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
		
		if(com->vt.vars[0].v.i && !com->vt.vars[1].v.i)
			printf("GR: PASS!\n");
		else 
		{
			if(!com->vt.vars[0].v.i) printf("GR 1: FAIL!\n");
			if( com->vt.vars[1].v.i) printf("GR 2: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // GE
		bytecode code[] = 
		{ 
			//test 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_GE }, // compare
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			//test2
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_GE }, // compare
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 0
			//test3
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_GE }, // compare
			{.code = BC_POP, { {.a1 =2} }}, // pop into address 0
			//end
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 2;
		com->cons[1].size = -1;
		com->cons[1].v.i = 1;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 0;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		com->vt.vars[2].size =-1;
		com->vt.vars[2].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i && com->vt.vars[1].v.i && !com->vt.vars[2].v.i)
			printf("GE: PASS!\n");
		else 
		{
			if(!com->vt.vars[0].v.i) printf("GE 1: FAIL!\n");
			if(!com->vt.vars[1].v.i) printf("GE 2: FAIL!\n");
			if( com->vt.vars[2].v.i) printf("GE 3: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // FEQUAL 
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_FEQUAL }, // compare
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 0
			//test2
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 2
			{.code = BC_CPUSH, { {.a1 =2} } }, //push const at address 1
			{.code = BC_FEQUAL }, // compare
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 1
			//test3
			{.code = BC_CPUSH, { {.a1 =3} } }, //push const at address 3
			{.code = BC_CPUSH, { {.a1 =4} } }, //push const at address 4
			{.code = BC_FEQUAL }, // compare
			{.code = BC_POP, { {.a1 =2} }}, // pop into address 2
			//test4
			{.code = BC_CPUSH, { {.a1 =5} } }, //push const at address 5
			{.code = BC_CPUSH, { {.a1 =6} } }, //push const at address 6
			{.code = BC_FEQUAL }, // compare
			{.code = BC_POP, { {.a1 =3} }}, // pop into address 3
			//test5
			{.code = BC_CPUSH, { {.a1 =0} } }, //push const at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push const at address 1
			{.code = BC_FEQUAL }, // compare
			{.code = BC_POP, { {.a1 =4 }}}, // pop into address 4
			//end
			{.code = BC_END}
		};
		
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.f = 1.0f;
		com->cons[1].size = -1;
		com->cons[1].v.f = 2.0f;
		
		com->cons[2].size = -1;
		com->cons[2].v.f  = 0.0f;
		for(int i = 0; i < 10; i++)
			com->cons[2].v.f += 0.01f;
		com->cons[2].v.f *= 10.0f;
		
		com->cons[3].size = -1;
		com->cons[3].v.f  = ((float)INT_MAX)*((float)INT_MAX)/(1000.0f);
		com->cons[4].size = -1;
		com->cons[4].v.f  = 0.001f*((float)INT_MAX)*((float)INT_MAX);
		
		com->cons[5].size = -1;
		com->cons[5].v.f  = 5.0f*FLT_MIN-2.0f*FLT_MIN;
		com->cons[6].size = -1;
		com->cons[6].v.f  = 3.0f*FLT_MIN;
		
		for(int i=0;i<5;i++) {
			com->vt.vars[i].size = -1;
			com->vt.vars[i].v.i = 0;
		}
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i && com->vt.vars[1].v.i && com->vt.vars[2].v.i 
			&& com->vt.vars[3].v.i && !com->vt.vars[4].v.i)
			printf("FEQUAL: PASS!\n");
		else 
		{
			if(!com->vt.vars[0].v.i)
				printf("FEQUAL 1: FAIL!\n");
			if(!com->vt.vars[1].v.i)
				printf("FEQUAL 2: FAIL!\n");
			if(!com->vt.vars[2].v.i)
				printf("FEQUAL 3: FAIL!\n");
			if(!com->vt.vars[3].v.i)
				printf("FEQUAL 4: FAIL!\n");
			if(com->vt.vars[4].v.i)
				printf("FEQUAL 5: FAIL!\n");
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
