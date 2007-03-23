#ifndef BULTINS_H_
#define BULTINS_H_

#include "bytecode.h"
#include <stdbool.h>

typedef struct _builtin_t{
  const char *name;
  short int ac; // arg count
  bool isFloat; //true if returen type and all args are float
		//false if they are all int
		//no other possible cases
  BC_DEF code;
} builtin;

//float compsl_rand(float);
static const int builtins_len = 21;
//ln,floor,ceil
static const builtin builtins[] = 
{
	{.name ="abs", .ac=1, .isFloat=false, .code=BC_ABS},
	{.name = "absf", .ac=1, .isFloat=true, .code=BC_ABSF},
	{.name = "sin", .ac=1, .isFloat=true, .code=BC_SIN},
	{.name = "cos", .ac=1, .isFloat=true, .code=BC_COS},
	{.name = "tan", .ac=1, .isFloat=true, .code=BC_TAN},
	{.name = "asin", .ac=1, .isFloat=true, .code=BC_ASIN},
	{.name = "acos", .ac=1, .isFloat=true, .code=BC_ACOS},
	{.name = "atan", .ac=1, .isFloat=true, .code=BC_ATAN},
	{.name = "sqrt", .ac=1, .isFloat=true, .code=BC_SQRT},
	{.name = "ln", .ac=1, .isFloat=true, .code=BC_LN},
	{.name = "floor", .ac=1, .isFloat=true, .code=BC_FLOOR},
	{.name = "ceil", .ac=1, .isFloat=true, .code=BC_CEIL},
	{.name = "rand", .ac=0, .isFloat=true, .code=BC_RAND},
	{.name = "atan2", .ac=2, .isFloat=true,.code=BC_ATAN2},
	{.name = "pow", .ac=2, .isFloat=true, .code=BC_POW},
	{.name = "min", .ac=2, .isFloat=true, .code=BC_MIN},
	{.name = "max", .ac=2, .isFloat=true, .code=BC_MAX},
	{.name = "minf", .ac=2, .isFloat=true, .code=BC_MINF},
	{.name = "maxf", .ac=2, .isFloat=true, .code=BC_MAXF},
	{.name = "hypot", .ac=2, .isFloat=true, .code=BC_HYPOT},
	{.name = "yes", .ac=0, .isFloat=false, .code=BC_PYES}
};
#endif /*BULTINS_H_*/
