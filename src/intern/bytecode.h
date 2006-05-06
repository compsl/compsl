#ifndef BYTECODE_H_
#define BYTECODE_H_

#include <inttypes.h>

typedef struct
{
  uint16_t code;
  union 
  {
    struct
    {
      uint8_t a1;
      uint8_t a2;
    };
    uint16_t a; // look up wether this works as expected
    int16_t sa;
  };
} bytecode;

typedef enum  { 
	BC_NOOP = 0, BC_PUSH, 	BC_APUSH, 	BC_CPUSH, 	// 01-03
	BC_POP, 	BC_APOP,	BC_DPOP, 	BC_SWAP, 	// 04-07
	BC_CALL, 	BC_ADD, 	BC_SUB, 	BC_MUL, 	//08-0B
	BC_DIV ,	BC_CMP,		BC_FADD,	BC_FSUB, 	//0C-0F
	BC_FMUL, 	BC_FDIV,	BC_FCMP,	BC_JMP, 	//10-13
	BC_JMPL,	BC_JMPE,	BC_JMPG,	BC_JMLE, 	//14-17
	BC_JMNE, 	BC_JMGE,	BC_FLIN,	BC_INFL, 	//18-1B
	BC_MOD,		BC_FMOD,	BC_GPSH,	BC_GAPS, 	//1C-1F
	BC_GPOP,	BC_GAPP,	BC_AND,		BC_OR,		//20-23
	BC_NOT, 	BC_BAND,	BC_BOR,		BC_BXOR,	//24-27
	BC_BNOT,	BC_SFTL,	BC_SFTR,
	BC_END, 	BC_HLT, 	BC_DBG
	} BC_DEF;
	

#endif /*BYTECODE_H_*/
