
// $Id$

/** @file compsl.h
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

// **************************************
// * Portability Stuff
// **************************************

#include <limits.h>
// DJGPP has no inttypes.h, or at least did not when I 
// tried building compsl for it
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

// **************************************
// * Error handling stuff               *
// **************************************

/** 
 * holds an error code, is the type of the errorno in VMs and compartments 
 */
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
 * @param err the error code
 * @return a string describing the error
 */
COMPSL_EXPORT const char *compsl_getErrstr(COMPSL_ERROR err);
/**
 * Print the message for the error code to stderr
 * @param err the error code
 */
COMPSL_EXPORT void compsl_printErr(COMPSL_ERROR err);

// **************************************
// * Vars                               *
// **************************************

/**
 * for variables (the actual value)
 */
typedef union {int32_t i; float f;} intfloat; 
  // TODO: it would be nice if we could guarantee sizeof(intfloat) == 32 bits

/**
 * Internal representation of variables, also used to pass to functions
 * added by application
 */
typedef struct _var_t
{
    union 
    {
       intfloat v; /// the value of a scalar
       intfloat *p; ///< the array (if var is an array)
    };
    
    int size; ///< for the size of the array, -1 if scalar
} var;

// **************************************
// * VM                                 *
// **************************************

/**
 * the type of functions that the compsl code can call, 
 * takes an array of vars, represent parameters in left to right order
 */
typedef intfloat (*compsl_nat)(var *);

/**
 * represents global state about a bunch of compartments
 * 
 * holds native functions and global vars
 */
typedef struct _VM_t VM;

/**
 * Make a new VM
 * @return a new VM or NULL on error
 */
COMPSL_EXPORT VM *createVM(void);

/**
 * Free up any memory allocated for a VM, and the VM itself
 * @param vm the VM to destroy
 */
COMPSL_EXPORT void destroyVM(VM *vm);

//NOTE: the string of the name of new vars is copied and the copy is retained by the VM
//      for the purpose of identifying the variable. Same goes for native functions.

// note that when adding vars if an identical var is already present a pointer to it 
// is simply returned, and no new var is added to the table. 


/* they return a pointer to the variable so that
 * the value can be set and read. These operate with a VM and therefor add
 * global variables.
 */
 
/** 
 * Add a new integer global variable, if a variable of the same name and type 
 * already exists it is returned and no new variable is created
 * 
 * @param vm the vm to add it to
 * @param name the name of the new variable 
 * @return pointer to the value of the variable, NULL on error
 */
COMPSL_EXPORT int32_t *vm_addInt(VM *vm, const char *name);

/** 
 * Add a new floating point global variable, if a variable of the same name and 
 * type already exists it is returned and no new variable is created
 * 
 * @param vm the vm to add it to
 * @param name the name of the new variable
 * @return pointer to the value of the variable, NULL on error
 */
COMPSL_EXPORT float *vm_addFloat(VM *vm, const char *name);

/**
 * Return a pointer to the named global variable so that
 * the value can be set and read, Note if a variable of a differnt type
 * with the given name exists it is NOT returned.
 * 
 * @param vm the vm to search
 * @param name the name of the variable
 * @return pointer to the value of the variable, NULL on error
 */
COMPSL_EXPORT float *vm_getFloat(VM *vm, const char *name);

/**
 * Return a pointer to the named global variable so that
 * the value can be set and read, Note if a variable of a differnt type
 * with the given name exists it is NOT returned.
 * 
 * @param vm the vm to search
 * @param name the name of the variable
 * @return pointer to the value of the variable, NULL on error
 */
COMPSL_EXPORT int32_t *vm_getInt(VM *vm, const char *name);



/**
 * add a native function to this vm, return true on success false otherwise
 * sets errno on fail
 * 
 * @param vm the vm to add to
 * @param func the function to call
 * @param retFloat true if the function returns a float, false if it returns an int
 * @param name the name of the function
 * @param isVoid true if the function does not return anything
 * @param params a list of types in format: [base type]([])
 * 	ex int array: int[]
 * 	ex int:		int
 * 	ex two ints and a float array: int int float[]
 * @return true on success false otherwise
 */
COMPSL_EXPORT bool addFunc(VM *vm, compsl_nat func, const char *name, 
			const char *params, bool retFloat, bool isVoid);


// **************************************
// * Compartment                        *
// **************************************

/**
 * represents a compartment, holds bytecode, local variables and constants
 * used in the code
 */
typedef struct _COMPART_t compart;

/**
 * Create a new compartment that uses the global state from the specified VM
 * ALL global variables declared in the compartment's code will be added to
 * this VM, and it will be searched for any native functions that are called
 * 
 * @param vm the VM to use
 * @return a new compartment, or NULL on error
 */
COMPSL_EXPORT compart *createComp(VM *vm);

/**
 * Clean up the compartment
 * 
 * @param com the compartment to clean up
 */
COMPSL_EXPORT void destroyComp(compart *com);

/** 
 * Add a new float variable to the compartment, if a variable of the same 
 * name and type already exists it is returned and no new variable is created
 * 
 * @param com the compartment to add it to
 * @param name the name of the new variable 
 * @return pointer to the value of the variable, NULL on error
 */
COMPSL_EXPORT float *com_addFloat(compart *com, const char *name);

/** 
 * Add a new integer variable to the compartment, if a variable of the same 
 * name and type already exists it is returned and no new variable is created
 * 
 * @param com the compartment to add it to
 * @param name the name of the new variable 
 * @return pointer to the value of the variable, NULL on error
 */
COMPSL_EXPORT int32_t *com_addInt(compart *com, const char *name);

/**
 * Return a pointer to the named local variable so that
 * the value can be set and read, Note if a variable of a differnt type
 * with the given name exists it is NOT returned.
 * 
 * @param com the compartment to search
 * @param name the name of the variable
 * @return pointer to the value of the variable, NULL on error
 */
COMPSL_EXPORT float *com_getFloat(compart *com, const char *name);

/**
 * Return a pointer to the named local variable so that
 * the value can be set and read, Note if a variable of a differnt type
 * with the given name exists it is NOT returned.
 * 
 * @param com the compartment to search
 * @param name the name of the variable
 * @return pointer to the value of the variable, NULL on error
 */
COMPSL_EXPORT int32_t *com_getInt(compart *com, const char *name);

/**
 * Get the id of the named cubbyhole, for use with runCubbyhole
 * 
 * @param com the compartment that contains the cubbyhole
 * @param name the name of the cubby
 * 
 * @return the id of the cubby or -1 on error
 */
COMPSL_EXPORT int16_t getCubbyID(compart *com, const char *name);


// **************************************
// * General stuff                      *
// **************************************

/**
 * Adds some handy native functions to the VM, assert type stuff.
 * 
 * testeqi(int, int) tests integers for equality
 * testeqf(float, float) test's floating point numbers for equality
 * test_reset() resets pass/fail counters
 * test_summary() prints a summary of the tests since the last test_reset
 * 
 * @param vm the VM to add the functions to
 */
COMPSL_EXPORT void addDebugLibToVm(VM *vm);

/**
 * Adds some handy native functions to the VM, output stuff.
 * printFloat(float) print a float to sdtout
 * printInt(int) print an int to stdout
 * printHello() print helloWorld to stdout
 * 
 * @param vm the VM to add the functions to
 */
COMPSL_EXPORT void addPrintLibToVm(VM *vm);

/**
 * compile a compartment from a file
 * 
 * @param filename the name of the file to read source from
 * @param out the compartment to put the cubbyholes and variables in
 */
COMPSL_EXPORT int fileCompile(const char *filename , compart *out);

/**
 * compile a compartment from a string in memory
 * 
 * @param code the string containing the sourcecode
 * @param out the compartment to put the cubbyholes and variables in
 */
COMPSL_EXPORT int stringCompile(const char *code, compart *out);

/**
 * Run a cubbyhole
 * 
 * @param com the compartment the cubbyhole is in
 * @param id the id of the cubbyhole
 */
COMPSL_EXPORT inline void runCubbyhole(compart *com, int id); // runs 

#ifdef __cplusplus
}
#endif

#endif /*COMPSL_H_*/
