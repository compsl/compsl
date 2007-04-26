/*              TSCTest.cpp                            © 2006-06-01 Agner Fog


Test program to measure how many clock cycles a little piece of code takes.

The program will execute the code to test NUMTESTS times and then print
out a list of clock counts.

The project consists only of this file

Instructions:

Insert the code to test at the place marked "### Test code here ###"

Compile and link for console mode under Windows, Linux or Mac OS X 
on x86 platform, 32 or 64 bits.

© 2005-2006 GNU General Public License www.gnu.org/copyleft/gpl.html

*/

#include <stdio.h>
#include "extern/compsl.h"

// ###################### DEFINE CONSTANTS ######################

// set number of times to run test
const int NUMTESTS = 20;

compart *com;
int16_t cubbyid;
const char *csl = 
"declare {int a,b,c,d,i; int stat;}\n\
 cubby foo {\n\
 	a=b=c=(int)(rand() * 10);\n\
 	d=3;\n\
 	if(a < 1)\n\
 	{\n\
 		a=d*2;\n\
 	} else\n\
 		d*=2;\n\
 	i=0;\n\
 	while(i < 1000) {\n\
 		if(b<a)\n\
 			c=a + b;\n\
 		else\n\
 			c= a - b;\n\
 		a = a + 1;\n\
 		i++;\n\
 	}\n\
 	a=b*2;\n\
 	c+=a*d-1;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 	a=b+c;\n\
 }";
// ###################### END DEFINE CONSTANTS ######################

#if ! defined(_MSC_VER) 
// define type __int64 if not defined
//typedef long long __int64;
#endif

#ifdef USE_ALIB    // use alibXXX.lib
// for compilers with insufficient inline assembly support, use external
// library for function ReadTSC()
#include "alib.h"

#else
// for compilers that understand the inline assembly code you can define
// the function ReadTSC() here:

// Read time stamp counter
// The return value is the internal clock count
uint64_t ReadTSC() {
   
   union {
   		uint32_t a[2];
   		uint64_t c;
   }res;                              // store 64 bit result here
   
   #if defined(__GNUC__) && !defined(__INTEL_COMPILER)
   // Inline assembly in AT&T syntax

   #if defined (_LP64)                      // 64 bit mode
      __asm__ __volatile__  (               // serialize (save rbx)
      "xorl %%eax,%%eax \n push %%rbx \n cpuid \n"
       ::: "%rax", "%rcx", "%rdx");
      __asm__ __volatile__  (               // read TSC, store edx:eax in res
      "rdtsc\n"
       : "=a" (res.a[0]), "=d" (res.a[1]) );
      __asm__ __volatile__  (               // serialize again
      "xorl %%eax,%%eax \n cpuid \n pop %%rbx \n"
       ::: "%rax", "%rcx", "%rdx");
   #else                                    // 32 bit mode
      __asm__ __volatile__  (               // serialize (save ebx)
      "xorl %%eax,%%eax \n pushl %%ebx \n cpuid \n"
       ::: "%eax", "%ecx", "%edx");
      __asm__ __volatile__  (               // read TSC, store edx:eax in res
      "rdtsc\n"
       : "=a" (res.a[0]), "=d" (res.a[1]) );
      __asm__ __volatile__  (               // serialize again
      "xorl %%eax,%%eax \n cpuid \n popl %%ebx \n"
       ::: "%eax", "%ecx", "%edx");
   #endif
   #else
   // Inline assembly in MASM syntax
      __asm {
         xor eax, eax
         cpuid                              // serialize
         rdtsc                              // read TSC
         mov dword ptr res, eax             // store low dword in res[0]
         mov dword ptr res+4, edx           // store high dword in res[1]
         xor eax, eax
         cpuid                              // serialize again
      };
   #endif   // __GNUC__
   
   return res.c;                   // return result
}

#endif   // USE_ALIB


void ClockTest (uint64_t clocks[]) {
   uint64_t before, overhead;
   int i;
   
   
	
   // dummy test loop without test code to measure overhead
   for (i=0; i < NUMTESTS; i++) {
      before = ReadTSC();                   // clock count before
      // no test code here, measure overhead only
      clocks[i] = ReadTSC() - before;       // clock count after
   }

   // find minimum overhead
   overhead = clocks[0];
   for (i=0; i < NUMTESTS; i++) {
      if (clocks[i] < overhead) overhead = clocks[i];
   }

   // loop to measure clock cycles of test code
   for (i=0; i < NUMTESTS; i++) {
      before = ReadTSC();                   // clock count before

// ###################### Test code here ######################

	runCubbyhole(com,cubbyid);
      
// ###################### Test code end  ######################

      clocks[i] = ReadTSC() - before;       // clock count after
   }

   // subtract overhead from clock counts
   for (i=0; i < NUMTESTS; i++) {
      clocks[i] -= overhead;
   }
}


int main() {
   // list of clock counts
   uint64_t clocklist[NUMTESTS];
   int i;
   
   	VM *vm=createVM();
	if(vm == NULL) { fprintf(stderr,"Error initializing compsl"); exit(1);}
	com=createComp(vm);
	if(com == NULL)
	{
		fprintf(stderr,"Error initializing compsl");
		exit(1);
	}
	stringCompile(csl, com);
	
	cubbyid = getCubbyID(com,"foo");

   // run tests
   ClockTest (clocklist);

   // print results
   printf ("\n  test     clock cycles");
   for (i = 0; i < NUMTESTS; i++) {
      printf ("\n%6i  %llu", i+1, clocklist[i]);
   }

   printf ("\n");
   return 0;
}
