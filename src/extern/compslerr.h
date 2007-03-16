#ifndef COMPSLERR_H_
#define COMPSLERR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _COMPSL_ERROR_TYPE_
{
	COMPSL_NOERR = 0,
	COMPSL_VARS_FULL,
	COMPSL_CUBBYS_FULL,
	COMPSL_FUNC_FULL,		//function table full
	COMPSL_NO_SUCH_CUBBY,
	COMPSL_NO_SUCH_VAR,
	COMPSL_NO_SUCH_FUNC,
	COMPSL_BAD_PARM_FRMT	//invalid native function parameter spec
} COMPSL_ERROR;

const char *compsl_getErrstr(COMPSL_ERROR err);
void compsl_printErr(COMPSL_ERROR err);

#ifdef __cplusplus
}
#endif

#endif /*COMPSLERR_H_*/
