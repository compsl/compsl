#ifndef COMPSLERR_H_
#define COMPSLERR_H_

typedef enum _COMPSL_ERROR_TYPE_
{
	COMPSL_NOERR = 0,
	COMPSL_VARS_FULL,
	COMPSL_CUBBYS_FULL,
	COMPSL_NO_SUCH_CUBBY,
	COMPSL_NO_SUCH_VAR
} COMPSL_ERROR;

const char *compsl_getErrstr(COMPSL_ERROR err);
void compsl_printErr(COMPSL_ERROR err);

#endif /*COMPSLERR_H_*/
