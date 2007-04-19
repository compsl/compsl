
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


#ifndef COMPSL_H_
#define COMPSL_H_
#include <stdbool.h>
#include <malloc.h>


// deal with symbol visibilitys
#ifdef _MSC_VER
	// can assume that we are NOT build library
	// since that would generate errors anyway, (gcc specific code)
	#define COMPSL_EXPORT __declspec(dllimport) __cdecl
	#define COMPSL_LOCAL
	#define COMPSL_INTERN
#elif defined(BUILDING_COMPSL)
	#ifdef WIN32
		#define COMPSL_EXPORT __declspec(dllexport) _cdecl
		#define COMPSL_LOCAL
		#define COMPSL_INTERN
	#elif (__GNUC__ > 3)
		#define COMPSL_EXPORT __attribute__ ((visibility("default")))
		#define COMPSL_LOCAL __attribute__ ((visibility("hidden")))
		#define COMPSL_INTERN __attribute__ ((visibility("internal")))
	#else
		#define COMPSL_EXPORT
		#define COMPSL_LOCAL
		#define COMPSL_INTERN
	#endif
#elif defined(WIN32)
	#define COMPSL_EXPORT __declspec(dllimport) _cdecl
	#define COMPSL_LOCAL
	#define COMPSL_INTERN
#else
	#define COMPSL_EXPORT
	#define COMPSL_LOCAL
	#define COMPSL_INTERN
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************
 * Portability Stuff
 ***************************************/
#include <limits.h>
#ifndef  DJGPP
#	include <inttypes.h>
#else
	typedef unsigned char  uint8_t;
	typedef signed char    int8_t;
	typedef unsigned short uint16_t;
	typedef short          int16_t;

/* 32-bit */
	typedef unsigned       uint32_t;
	typedef int            int32_t;
#  endif

/***************************************
 * Error handling stuff
 ***************************************/
typedef enum _COMPSL_ERROR_TYPE_
{
	COMPSL_NOERR = 0,
	COMPSL_NOMEM,
	COMPSL_VARS_FULL,
	COMPSL_CONST_FULL,
	COMPSL_CUBBYS_FULL,
	COMPSL_FUNC_FULL,		//function table full
	COMPSL_NO_SUCH_CUBBY,
	COMPSL_NO_SUCH_VAR,
	COMPSL_NO_SUCH_FUNC,
	COMPSL_BAD_PARM_FRMT,	//invalid native function parameter spec
	COMPSL_NULL_POINTER		//got passed a NULL pointer
} COMPSL_ERROR;

/**
 * Return a string describing the error code
 * err the error code
 */
COMPSL_EXPORT const char *compsl_getErrstr(COMPSL_ERROR err);
/**
 * print the message for the error code to stderr
 */
COMPSL_EXPORT void compsl_printErr(COMPSL_ERROR err);

/***************************************
 * Vars
 ***************************************/

/**
 * for variables (the actual value)
 */
typedef union {int32_t i; float f;} intfloat; 
  // TODO: it would be nice if we could guarantee sizeof(intfloat) == 32 bits

/**
 * Internal representation of variables
 */
typedef struct _var_t
{
    union 
    {
       intfloat v; // the value of a scalar
       intfloat *p; // the array (if var is an array)
    };
    
    int size; // for the size of the array, -1 if scalar
} var;


struct SYMTABLE_T // don't bother with typedef, only here so I can sizeof it later
{
	uint16_t id;
	uint8_t typeflags;// OR together the apropriat flags ex float array = FLOAT_VAR | IS_ARRAY
	char *name; // symbol name
};

/**
 * Holds variables and their symbols, also tracks # of variables in it
 */
typedef struct VAR_TABLE_T
{
	uint16_t capacity; // max variables for this table
	uint16_t cnt; // number of variables
    var *vars; // the variables themselves
    struct SYMTABLE_T *symbols; // the symbol table for the vars
} varTable;


/***************************************
 * VM
 ***************************************/
 
#define VM_STACK_SIZE 256
#define VM_MAX_GVARS 256
#define VM_NATIVEFN_INIT_SIZE 64

/**
 * the type of functions that the compsl code can call, 
 * takes an array of vars, represent parameters in (left to right)? order
 */
 
 // TODO: above comment may be wrong about order
typedef intfloat (*compsl_nat)(var *);

/**
 * looks after one native function,holds pointer to function and
 * info about number and type of params
 */
typedef struct _nativeFN_t
{
        const char *name; // name of this function
        compsl_nat func; // pointer the function to call
        
        bool isVoid; //true iff function does not return a value
        bool retFloat; // true iff the return type is a float
        
        var *params; // the list of paramaters to pass to it
        uint8_t *paramFlags;// OR together the apropriat flags ex float array = FLOAT_VAR | IS_ARRAY
        uint8_t numParam; // how many paramaters to pass it
} nativeFN;

/**
 * represents global state about a bunch of compartments
 * 
 * holds native functions and global vars
 */
typedef struct _VM_t
{
    nativeFN *natives; // native function table
    uint16_t ncnt; // number of native functions
    
    varTable vt;
    
    COMPSL_ERROR errorno; // holds the error code of the last error generated by this VM
} VM;

/**
 * Make a new VM, returns NULL on error
 */
COMPSL_EXPORT VM *createVM(void);

/**
 * Free up any memory allocated for a VM, and the VM itself
 */
COMPSL_EXPORT void destroyVM(VM *vm);

//NOTE: the string of the name of new vars is copied and the copy is retained by the VM
//      for the purpose of identifying the variable. Same goes for native functions.

// note that when adding vars if an identical var is already present a pointer to it 
// is simply returned, and no new var is added to the table. 

/* These two add variables, they return a pointer to the variable so that
 * the value can be set and read. These operate with a VM and therefor add
 * global variables. 
 * 
 * Return NULL on error, if a variable of the same name and type already exists
 * it is returned and no new variable is created 
 */

COMPSL_EXPORT int32_t *vm_addInt(VM *vm, const char *name);
COMPSL_EXPORT float *vm_addFloat(VM *vm, const char *name);// add a float to the vm's global vars, and return a pointer to it

/* Return a pointer to the named global variable so that
 * the value can be set and read, Note if a variable of a differnt type
 * with the given name exists it is NOT returned.
 */
COMPSL_EXPORT float *vm_getFloat(VM *vm, const char *name);
COMPSL_EXPORT int32_t *vm_getInt(VM *vm, const char *name);

/** 
 * Represents one paramenter to a native function.
 */
typedef struct native_param_t
{
	bool isFloat; // true if the param has type float, false if it's an int
	bool isArray; // if the param is an array true, false if it's scalar
} nativeParam;

/**
 * add a native function to this vm, return true on success false otherwise
 * sets errno on fail
 * 
 * vm the vm to add to
 * func the function to call
 * retFloat true if the function returns a float, false if it returns an int
 * name the name of the function
 * params a list of types in formate:
 * 		<base type>([])
 * ex int array: int[]
 * ex int:		int
 * ex two ints and a float array: int int float[]
 */
COMPSL_EXPORT bool addFunc(VM *vm, compsl_nat func, const char *name, const char *params, bool retFloat, bool isVoid);


/***************************************
 * Compartment
 ***************************************/

#define COMPART_MAX_VARS 256
#define COMPART_MAX_CONSTS 256
#define COMPART_MAX_CUBBYS 16

typedef struct _COMPART_t
{
    varTable vt;
    //varTable ct;
    var cons[COMPART_MAX_CONSTS];
    uint16_t numConst;
    uint16_t numCubbys;
    
    struct _CUBBY_t
    {
    	void *code; // pointer to the bytecode of this cubby
    	char *name; // name of this cubby
    } cubbys[COMPART_MAX_CUBBYS];
    
    VM *vm; // the vm this compartment was compiled with
    
    COMPSL_ERROR errorno; // error code of last error produced by this compartment
} compart;

/**
 * Create a new compartment that uses the global state from the specified VM
 * 	ALL global variables declared in the compartment's code will be added to
 * 	this VM, and it will be searched for any native functions that are called
 */
COMPSL_EXPORT compart *createComp(VM *vm);
/**
 * Clean up the compartment
 */
COMPSL_EXPORT void destroyComp(compart *);

/*Add a float variable to the compartment
 * returns NULL on error, sets compartments errorno
 * 	com the compartment to add the variable to
 * 	name the name of the variable to add
 */
float *com_addFloat(compart *com, const char *name);

/*Add an int variable to the compartment
 * returns NULL on error, sets compartments errorno
 * 	com the compartment to add the variable to
 * 	name the name of the variable to add
 */
COMPSL_EXPORT int32_t *com_addInt(compart *com, const char *name);

/*Return a pointer to the named float variable
 * returns NULL on error, sets compartments errorno
 * 	com the compartment to search
 * 	name the name of the variable to find
 */
COMPSL_EXPORT float *com_getFloat(compart *com, const char *name);

/*Return a pointer to the named int variable
 * returns NULL on error, sets compartments errorno
 * 	com the compartment to search
 * 	name the name of the variable to find
 */
COMPSL_EXPORT int32_t *com_getInt(compart *com, const char *name);

/*Get the id of the named cubbyhole, for use with runCubbyhole
 * returns -1 on error, sets compartment's errorno
 * 
 * com the compartment that contains the cubbyhole
 * name the name of the cubby
 */
COMPSL_EXPORT int16_t getCubbyID(compart *com, const char *name);


/***************************************
 * General stuff
 ***************************************/

/**
 * Adds some handy native functions to the VM, assert type stuff.
 * 
 * testeqi(int, int) tests integers for equality
 * testeqf(float, float) test's floating point numbers for equality
 * test_reset() resets pass/fail counters
 * test_summary() prints a summary of the tests since the last test_reset
 */
COMPSL_EXPORT void addDebugLibToVm(VM *);
/**
 * Adds some handy native functions to the VM, output stuff.
 * printFloat(float) print a float to sdtout
 * printInt(int) print an int to stdout
 * printHello() print helloWorld to stdout	
 */
COMPSL_EXPORT void addPrintLibToVm(VM *);

COMPSL_EXPORT int fileCompile(const char *filename , VM*, compart*);
COMPSL_EXPORT int stringCompile(const char *code, VM*, compart*);

COMPSL_EXPORT inline void runCubbyhole(compart *com, int id); // runs 

#ifdef __cplusplus
}
#endif

#endif /*COMPSL_H_*/
