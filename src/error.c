#include "extern/compslerr.h"
#include <stdio.h>

static char *emessgs[] =
{//TODO: fill in errors
	"No Error",
	"",
	"",
	""
};

//static char *noSuchMessage = "Non-existant error message";

char *compsl_getErrstr(COMPSL_ERROR err)
{
	//TODO: find some way to bounds check this
	return emessgs[err];
}

void compsl_printErr(COMPSL_ERROR err)
{
	//TODO: find some way to bounds check this
	fprintf(stderr, "ERROR %i: %s", err, emessgs[err]);
}
