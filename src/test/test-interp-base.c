#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <float.h>
#include <math.h>

#include "intern/bytecode.h"
#include "compsl.h"
#include "intern/vm.h"
#include "intern/compartment.h"


/*******************************************************
 * Tester for the most basic instructions (stack operations)
 *******************************************************/

int main()
{
	puts("Running base interpreter tests");
	
	VM *vm = createVM();
	compart *com = createComp(vm);
	bool ret = 0;
	
	/************************ Begin Tests *************************************/
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
	
	{ // DPOP
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_PUSH, { {.a1 =1} } }, //push local at address 1
			{.code = BC_DPOP, { {.a1 =1} }}, // pop into address 1
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
			printf("DPOP: PASS!\n");
		else {
			printf("DPOP: FAIL!\n");
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
	
	{ // STO
		bytecode code[] = 
		{ 
			{.code = BC_PUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_STO, { {.a1 =1} }}, // pop into address 1
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
			printf("STO: PASS!\n");
		else {
			printf("STO: FAIL!\n");
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
	
	// GPSH, GPOP
	
	// GAPS
	
	// GAPP
	
	// GSTO	//save top of stack to global variable without moving stack pointer
	
	{ // SAVE, PSHT
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push local at address 0
			{.code = BC_SAVE},
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 1
			{.code = BC_PSHT},
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 1
			{.code = BC_END}  // end
		};
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 5;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[1].v.i == com->vt.vars[0].v.i)
			printf("SAVE, PSHT: PASS!\n");
		else {
			printf("SAVE, PSHT: FAIL!\n");
			ret = 1;
		}
	}
	
	{ // POPT, PSHT
		bytecode code[] = 
		{ 
			{.code = BC_CPUSH, { {.a1 =0} } }, //push local at address 0
			{.code = BC_CPUSH, { {.a1 =1} } }, //push local at address 0
			{.code = BC_POPT},
			{.code = BC_POP, { {.a1 =0} }}, // pop into address 1
			{.code = BC_PSHT},
			{.code = BC_POP, { {.a1 =1} }}, // pop into address 1
			{.code = BC_END}  // end
		};
		com->cubbys[0].code = code;
		
		com->cons[0].size = -1;
		com->cons[0].v.i = 1;
		com->cons[1].size = -1;
		com->cons[1].v.i = 5;
		
		com->vt.vars[0].size =-1;
		com->vt.vars[0].v.i = 2;
		com->vt.vars[1].size =-1;
		com->vt.vars[1].v.i = 0;
		
		runCubbyhole(com, 0);
		
		if(com->vt.vars[0].v.i == com->cons[0].v.i
		    	&& com->vt.vars[1].v.i == com->cons[1].v.i)
			printf("POPT, PSHT: PASS!\n");
		else {
			printf("POPT, PSHT: FAIL!\n");
			ret = 1;
		}
	}
	
	/************************** End Tests *************************************/
	com->cubbys[0].code = NULL;
	
	destroyComp(com);
	destroyVM(vm);
	return ret; //return 0 else make's unhappy
}
