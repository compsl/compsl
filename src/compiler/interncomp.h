
#ifndef INTERNCOMP_H_
#define INTERNCOMP_H_

#include "node.h"
#include "../extern/port.h"
#include "../intern/bytecode.h"
#include "../intern/vars.h"
#include "../intern/builtins.h"
#include "../intern/gen.h"
#include "../intern/compartment.h"


#define CONTINUE_NOOP_IDEN 89
#define BREAK_NOOP_IDEN 90


// Compiler is not reentrant
// TODO: Make compiler reentrant
// Step to make reentrants: 
//   - Find out how to store dynamic vars
//   - Store the three vars below in those vars

extern uint8_t ZERO_CONSTANT;
extern compart *ccompart;
extern char *sprt;


typedef struct _expression_t {
  bool isLiteral;
  bool isFloat;	
  union { 
    int in; 
    float fl; 
    bytecode* bcode; 
  } val;
} expression;


// binops.c
expression* bin_op(int op,expression* a, expression* b);

// functions.c
expression* function_call(const char* name, list *params);

// interncomp.c
bytecode* expr_toBc(expression *exp);
void expr_free(expression* exp);
void expr_ensureLit(expression* exp);
void expr_autocast(bool toFloat,expression *e);
int bc_len(bytecode *);

// var.c
expression *assignVar(const char *str, expression *e);
expression *readVar(const char* name);

// control.c
bytecode *ctrlWhile(expression *cond, bytecode *block);
bytecode *ctrlIf(expression *cond, bytecode *block, bytecode *elseBlock);

// err.c
void internalCompileError(const char* str);
void compileError(const char *str);
void compileWarning(const char *str);


#endif //INTERNCOMP_H_
