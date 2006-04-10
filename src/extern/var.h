#ifndef VAR_H_
#define VAR_H_
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union {int32_t i; float f;} intfloat; 

typedef struct _var_t
{
    union 
    {
       intfloat v;
       intfloat *p;
    };
    
    int size; // for the size of the array
} var;


struct SYMTABLE_T // don't bother with typedef, only here so I can sizeof it later
{
	uint16_t id;
	uint8_t typeflags;// OR together the apropriat flags ex float array = FLOAT_VAR | IS_ARRAY
	char *name;
};

typedef struct VAR_TABLE_T
{
	uint16_t cnt; // number of variables
    var *vars; // the variables themselves
    struct SYMTABLE_T *symbols; // the symbol table for the vars
} varTable;

#ifdef __cplusplus
}
#endif

#endif /*VAR_H_*/
