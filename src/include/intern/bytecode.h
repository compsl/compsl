#ifndef COMPSL_BYTECODE_H_
#define COMPSL_BYTECODE_H_

#include "compsl.h"


// all instructions working with variables take address in a1 unless otherwise
// stated

COMPSL_INTERN typedef enum  {
	BC_NOOP,
	BC_PUSH,
	BC_PSHT,
	BC_APUSH,
	BC_CPUSH,
	BC_GPSH,
	BC_GAPS,
	BC_POP,
	BC_POPT,
	BC_APOP,
	BC_DPOP,
	BC_GPOP,
	BC_GAPP,
	BC_DUP,
	BC_CALL,
	BC_ADD,
	BC_SUB,
	BC_MUL,
	BC_DIV,
	BC_LE,
	BC_LS,
	BC_EQ,
	BC_NE,
	BC_GR,
	BC_GE,
	BC_MOD,
	BC_FMOD,
	BC_AND,
	BC_OR,
	BC_NOT,
	BC_BAND,
	BC_BOR,
	BC_BXOR,
	BC_BNOT,
	BC_SFTL,
	BC_SFTR,
	BC_FADD,
	BC_FSUB,
	BC_FMUL,
	BC_FDIV,
	BC_FLE,
	BC_FL,
	BC_FEQ,
	BC_FNE,
	BC_FGR,
	BC_FGE,
	BC_JMP,
	BC_JMZ,
	BC_JMN,
	BC_FLIN,
	BC_INFL,
	BC_SAVE,
	BC_STO,
	BC_GSTO,
	BC_INC,
	BC_DEC,
	BC_FINC,
	BC_FDEC,
	BC_CHOOSE,
	BC_ABS,
	BC_ABSF,
	BC_SIN,
	BC_COS,
	BC_TAN,
	BC_ASIN,
	BC_ACOS,
	BC_ATAN,
	BC_SQRT,
	BC_LN,
	BC_FLOOR,
	BC_CEIL,
	BC_RAND,
	BC_ATAN2,
	BC_POW,
	BC_MIN,
	BC_MAX,
	BC_MINF,
	BC_MAXF,
	BC_HYPOT,
	BC_FEQUAL,
	BC_PYES,
	BC_NONO,
	BC_END,
	BC_HLT,
	BC_DBG,
BC_NO_BYTECODE
} BC_DEF;

typedef struct
{
  BC_DEF code;
  union 
  {
    struct
    {
      uint8_t a1 /*__deprecated__*/; // uncomment depreciated after first relase
      uint8_t a2 /*__deprecated__*/; // clean up all refernces to it
    };
    uint16_t a; // look up wether this works as expected
    int16_t sa;
  };
} bytecode;

#endif /*BYTECODE_H_*/
