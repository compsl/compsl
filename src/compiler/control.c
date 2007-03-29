#include <string.h>
#include <assert.h>
#include "interncomp.h"


bytecode *ctrlIf(expression *condExpr, bytecode *block, bytecode *elseBlock) {
  // Plan: condition, cjmp on 0, block, jmp to end, else
  int len, cpos; 
  
  bytecode *cond = expr_toBc(condExpr);
  bytecode *tmp = NULL;
  assert(cond != NULL);
  
  int testLen = 1;
  int jmpLen = 1;
  int condLen = bc_len(cond);
  int blockLen = bc_len(block);
  int elseLen = ((elseBlock !=(bytecode*)0)?bc_len(elseBlock):0);
  
  assert(condLen>0);
  
  len = condLen+testLen+blockLen+1;
  if(elseLen>0) 
    len+=jmpLen + elseLen;
  
  cpos = 0;
  
  // Calculate the condition value
  tmp = realloc(cond, sizeof(bytecode)*len);
  if(tmp == NULL)
  { 
  	free(cond);
  	expr_free(condExpr);
 	free(block);
 	if(elseBlock!=NULL) free(elseBlock);
  	internalCompileError("Out of Memory");
  }
  cond = tmp;
  cpos += condLen;
  
  // Jump if false
  cond[cpos].code = BC_JMZ;
  cond[cpos].sa = blockLen +1;
  
  // We need to jump past the second jump as well
  if(elseLen>0)
    cond[cpos].sa++;
  
  cpos+=testLen;
  
  memcpy(&cond[cpos], block, sizeof(bytecode)*blockLen);
  cpos+=blockLen;
  
  if(elseLen>0) {
    cond[cpos].code = BC_JMP;
    cond[cpos].sa = elseLen+1;
    cpos++;
    
    memcpy(&cond[cpos], elseBlock, sizeof(bytecode)*elseLen);
    cpos+=elseLen;
  }
  
  cond[cpos].code = BC_NONO;
  cpos++;
  
  assert(cpos == len);

  // Free parameters
  condExpr->isLiteral = true;
  expr_free(condExpr);
  free(block);
  if(elseBlock!=NULL) free(elseBlock);
  
  return cond;
}


void fixBreaksContinues(bytecode* bc, int len, int testLen, int jmpLen);

bytecode *ctrlWhile(expression *condExpr, bytecode *block) {
  // Plan: 
  //
  //   condition
  //   test: jmp on false +(blockLen+jmpLen+1)
  //   block
  //   jmp: jmp -(blocklen+condLen+testLen)
  //
  int len, cpos; 
  
  bytecode *cond = expr_toBc(condExpr);
  bytecode *tmp = NULL;
  assert(cond!=NULL);
  
  int condLen = bc_len(cond);
  int testLen = 1;
  int blockLen = bc_len(block);
  int jmpLen = 1;

  assert(condLen>0);

  len = condLen+testLen+blockLen+jmpLen+1;
  
  cpos = 0;
  
  // Calculate the condition value
  tmp = realloc(cond, sizeof(bytecode)*len); // why does it die here on windows?
  if(tmp == NULL)
  {
  	free(cond);
    condExpr->isLiteral = true;
    expr_free(condExpr);
    free(block);
    internalCompileError("Out of Memory");
  }
  cond = tmp;
  cpos += condLen;
		  
  cond[cpos].code = BC_JMZ;
  cond[cpos].sa = blockLen+jmpLen+1;
  cpos+=testLen;
  
  fixBreaksContinues(block, blockLen, condLen+testLen, jmpLen);

  // block
  memcpy(&cond[cpos], block, sizeof(bytecode)*blockLen);
  cpos+=blockLen;
  
  // jmp
  cond[cpos].code = BC_JMP;
  cond[cpos].sa = -(blockLen+condLen+testLen);
  cpos++;


  // End
  cond[cpos].code = BC_NONO;
  cpos++;

  assert(cpos == len);


  // Now we must search for break and continue keywords
  

  // Free parameter
  condExpr->isLiteral = true;
  expr_free(condExpr);
  free(block);

  return cond;
}

/*
 * bc  - the block
 * len - length of block
 * condLen - length of cond
 *
 * Foreach NOOP bytecode we need to transform into a JMP to the correct place
 * CONTINUE_NOOP_IDEN, BREAK_NOOP_IDEN
 */
void fixBreaksContinues(bytecode* bc, int len, int testLen, int jmpLen) {
  assert(bc[len].code == BC_NONO || bc[len].code == BC_END);

  for(int i=0;i<len;i++) {
    if(bc[i].code == BC_NOOP) {
      if(bc[i].a == CONTINUE_NOOP_IDEN) {

	// Jump to before cond
	bc[i].code=BC_JMP;
	bc[i].sa= -(i+testLen);

      } else if(bc[i].a == BREAK_NOOP_IDEN) {

	// Jump to after end of block
	bc[i].code=BC_JMP;
	bc[i].sa= len+jmpLen-i;

      } else {
	    compileWarning("Produced code with a NOOP");
      }
    }
  }
  
}
