// $Id$

/*
    CompSL scripting language 
    Copyright (C) 2007  Thomas Jones & John Peberdy

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <assert.h>
#include <stdio.h>
#include "extern/compsl.h"
#include "intern/vars.h"

///////////////////////////
// Test lib stuff        //
///////////////////////////
int testCnt=1;
int failCnt=0;


intfloat compsl_testEqf(var *args) { 

  if(!var_testEqf(&args[0],&args[1])) {
    printf("TEST #%i FAILED (%f,%f)\n",testCnt, args[0].v.f, args[1].v.f); 
    failCnt++;
  }
  testCnt++;
  return (intfloat)0;
}

intfloat compsl_testEqi(var *args) { 

  if(!var_testEqi(&args[0],&args[1])) {
    printf("TEST #%i FAILED (%i,%i)\n",testCnt, args[0].v.i,args[1].v.i); 
    failCnt++;
  }
  testCnt++;
  return (intfloat)0;
}

intfloat compsl_testReset(var *args) { 
  testCnt=1; 
  failCnt=0;
  return (intfloat)0;
}

intfloat compsl_testSummary(var *args) { 
  if(failCnt==0)
    printf("PASS (%i tests)\n",testCnt-1);
  else 
    printf(">> FAIL << (%i tests completed, %i failed)\n",testCnt-1, failCnt);
  return (intfloat)0;
}


///////////////////////
// Print lib stuff   //
///////////////////////
intfloat compsl_printFloat(var *args) { printf("Value: %f\n", args[0].v.f); return (intfloat)0;}
intfloat compsl_printInt(var *args) { printf("Value: %i\n", args[0].v.i); return (intfloat)0;}	
intfloat compsl_printHello(var *args) { printf("Hello world\n"); return (intfloat)0;}	




////////////////////////////////////
// Actual addFunct calls          //
////////////////////////////////////
COMPSL_EXPORT void addDebugLibToVm(VM *vm) {
  assert(addFunc(vm, &compsl_testEqi, "testeqi", "int int",false,true));
  assert(addFunc(vm, &compsl_testEqf, "testeqf", "float float",false,true));
  assert(addFunc(vm, &compsl_testReset, "test_reset", "",false,true));
  assert(addFunc(vm, &compsl_testSummary, "test_summary", "",false,true));
}

COMPSL_EXPORT void addPrintLibToVm(VM *vm)
{
  assert(addFunc(vm, &compsl_printFloat, "printf", "float",false,true));
  assert(addFunc(vm, &compsl_printInt, "printi", "int",false,true));
  assert(addFunc(vm, &compsl_printHello, "helloWorld", "",false,true));
}
