#include "extern/compsl.h"
#include <stdio.h>

static const char *emessgs[] =
{//TODO: fill in errors
	"No Error",
	"Out of Memory",
	"VARIABLE TABLE FULL: Cannot add variable",
	"CONSTANT TABLE FULL: Cannot add constant",
	"Cubbyhole Table full: Too many cubbyholes",
	"Function Table full: Too many functions",
	"No Such Cubbyhole: the specified name does not exist",
	"No Such Variable: the specified name does not exist",
	"No Such Function: the specified name does not exist",
	"Cannot add function: Bad Function Parameter Format",
	NULL
};

//static char *noSuchMessage = "Non-existant error message";

COMPSL_EXPORT const char *compsl_getErrstr(COMPSL_ERROR err)
{
	//TODO: find some way to bounds check this
	return emessgs[err];
}

COMPSL_EXPORT void compsl_printErr(COMPSL_ERROR err)
{
	//TODO: find some way to bounds check this
	fprintf(stderr, "ERROR %i: %s", err, emessgs[err]);
}

