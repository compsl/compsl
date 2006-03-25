#include "extern/compslerr.h"
#include <stdio.h>

static const char *emessgs[] =
{//TODO: fill in errors
	"No Error",
	"VARIABLE TABLE FULL: Cannot add variable",
	"Cubbyhole Table full: Too many cubbyholes",
	"No Such Cubbyhole: the specified name does not exist",
	"No Such Variable: the specified name does not exist",
	""
};

//static char *noSuchMessage = "Non-existant error message";

const char *compsl_getErrstr(COMPSL_ERROR err)
{
	//TODO: find some way to bounds check this
	return emessgs[err];
}

void compsl_printErr(COMPSL_ERROR err)
{
	//TODO: find some way to bounds check this
	fprintf(stderr, "ERROR %i: %s", err, emessgs[err]);
}
