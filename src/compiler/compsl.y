 	


%parse-param { char const *file_name };
%initial-action
{	
	//printf("Parsing file: %s",file_name);
};


%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "node.h"
#include "compsl.tab.h"
#include "../intern/gen.h"
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../intern/compartment.h"
#include "../intern/bytecode.h"
#include "../intern/builtins.h"
//#include "../intern/debug.h"

#define YYERROR_VERBOSE 1

  // NOTE: ANY INCLUDES HERE SHOULD ALSO GO IN compsl.l probably


  extern FILE *yyin;
  extern compart *ccompart;
  char *sprt;
  
  int goparse(const char* fn, compart *com) {
    FILE* input;

    input = fopen(fn, "r");
    if(NULL == input) {
      DPRINTF("\n\n>> COULDN'T OPEN INPUT FILE - %s\n",fn);
      return -1;
    }
    yyin = input;
    
    DPRINTF("\n\n>> STARTING PARSE - %s\n",fn);
    ccompart = com;
    
    yyrestart(yyin);
    sprt=malloc(1024 * sizeof(char));
    int ret = yyparse(fn);
    DPRINTF(">> DONE PARSE\n\n");
    free(sprt);
    return ret;
  }
  int fileCompile(const char *filename , VM* vm, compart** out) {
    compart* com = createComp(vm);
    *out = com;
    return goparse(filename, com);
  }
  int stringCompile(char *code, size_t len, VM* vm, compart** out) {
    *out = (compart*)0;
    return -1;
  }

void yyerror(const char *fn, const char *msg) {
    fprintf(stderr,"> In file \"%s\"\n  Error: \"%s\"\n  Line Num: %i\n",fn,msg,-1);//yylloc.first_line);
	exit(2);
}

int yywrap(void) {
        return 1;
} 


  


////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////

// Binary operator helpers for bin_op()

expression* bin_bc_op(int op,expression* a, expression* b) {
  assert((a!=(expression*)0) && (b!=(expression*)0));
  bool isFloat;
  int alen,blen, endi;
  bytecode* mcode;

  // Change both sides to non-literals
  a->val.bcode = expr_toBc(a);
  a->isLiteral = false;
  b->val.bcode = expr_toBc(b);
  b->isLiteral = false;
  if(!a->isFloat && !b->isFloat) {
    isFloat = false;
  } else {
    autocast(true, a);
    autocast(true, b);
    isFloat = true;
  } 

  alen = bc_len(a->val.bcode);
  blen = bc_len(b->val.bcode);
  endi = alen+blen;

  mcode = malloc(sizeof(bytecode)*(alen+blen+2));

  memcpy(mcode, a->val.bcode,sizeof(bytecode)*alen);
  memcpy(&mcode[alen], b->val.bcode,sizeof(bytecode)*blen);  

  switch(op) {
  case PLUS:
    if(isFloat) 
      mcode[endi].code = BC_FADD;
    else
      mcode[endi].code = BC_ADD;
    break;
  case MINUS:
    if(isFloat) 
      mcode[endi].code = BC_FSUB;
    else
      mcode[endi].code = BC_SUB;
    break;
  case MULT:
    if(isFloat)
      mcode[endi].code = BC_FMUL;
    else 
      mcode[endi].code = BC_MUL;
    break;
  case DIV:
    if(isFloat) 
      mcode[endi].code = BC_FDIV;
    else
      mcode[endi].code = BC_DIV;
    break;
  default:
    sprintf(sprt,"Operation \"%i\" not implemented",op);
    compileError(sprt);
    return (expression*)0;
  }
  mcode[endi].a1 = 0;
  mcode[endi+1].code = BC_NONO;
  mcode[endi+1].a1=0;
  
  free(a->val.bcode);
  
  expr_free(b);
  a->val.bcode = mcode;
  return a;
	//TODO: this
}


expression* bin_lit_op(int op, expression* a, expression* b) {
	if(!a->isFloat && !b->isFloat) {
		if(op==PLUS)
		  a->val.in+=b->val.in;
		else if(op==MINUS)
		  a->val.in-=b->val.in;
		else if(op==MULT)
		  a->val.in*=b->val.in;
		else if(DIV)
		  a->val.in/=b->val.in;
		else {
		  internalCompileError("Unknown operator in bin_lit_op()");
		  return 0;
		}
		free(b);
		return a;
	}
	else {
		float n1 = (a->isFloat)?a->val.fl:((float)a->val.in);
		float n2 = (b->isFloat)?b->val.fl:((float)b->val.in);
		if(op==PLUS)
		  n1+=n2;
		else if(op==MINUS)
		  n1-=n2;
		else if(op==MULT)
		  n1*=n2;
		else if(op==DIV)
		  n1/=n2;
		else {
			puts("Unknown op");
			exit(1); //TODO: this
		}
		a->isFloat=true;
		a->val.fl=n1;
		free(b);
		return a;
	}
}


// Post: dont use a or b, they're freed if need be

expression* bin_op(int op,expression* a, expression* b) {
  assert((a!=(expression*)0) && (b!=(expression*)0));
	if(a->isLiteral && b->isLiteral) {
		return bin_lit_op(op,a,b);
	}
	else {
		return bin_bc_op(op,a,b);
	}
}


void compileError(const char *str) {
  fflush(stderr);
  fflush(stdout);
  fprintf(stderr,"Compile error: %s\n",str);
}

 void compileWarning(const char *str) {
  fflush(stderr);
  fflush(stdout);
  fprintf(stderr,"Compile warning: %s\n",str);
 }


///////////////////////////////////////////////////////////////////////
// END FUNC DECLS

%}

%union {
	int ival;
	float fval;
	char *sval;
	node *nval;
	bool bval;
	expression *expr;
	list *xlist;
	bytecode *bc;
}

%token <fval> FLOAT_LIT;
%token <ival> INT_LIT; 
%token <sval> IDENTIFIER;
%token CUBBY GLOBAL INT BOOL FLOAT TRUE FALSE IF ELSEIF ELSE WHILE BREAK RETURN SEMI COMA OPENB CLOSEB OPENP CLOSEP ISEQ ISNEQ ASSIGN ISGEQ ISLEQ ISGT ISLT NOT AND OR DECLARE PLUS MINUS MULT DIV MOD CLOSES OPENS CONTINUE; 
%type <expr> expression math retable;
%type <ival> cast post_modifier;
%type <nval> file header_stuff do_declare cubbys cubby decl;
%type <sval> cubby_id;
%type <bval> global_modifier intfloat_keyword;
%type <bc> stmt block if else control;
%type <xlist> paramlist stmts moreparamlist decls ident_list more_ident_list;

%locations

%%
file:
		header_stuff do_declare cubbys
        ;

header_stuff: 
		;
		
do_declare: {
		DPRINTF("Doing declare\n");
		}
		DECLARE OPENB decls CLOSEB {
		  DPRINTF("Done declare\n");
		}

cubbys:
		cubby cubbys | ;
cubby:	
		cubby_id block {
			com_addCubby(ccompart, $2, $1);
		}
cubby_id:
		CUBBY IDENTIFIER {
			DPRINTF("> Cubby: %s\n",$2);
			$$=$2;
		}

		
block:
		OPENB stmts CLOSEB {
			DPRINTF("Appending %i statements\n",$2->length);
			int len=0;
			llist *cur=$2->head;
			while($2->length>0 && cur) {
			  assert(cur->obj!=(expression*)0); 
			  len+=bc_len(cur->obj);
			  cur=cur->next;
			}
			
			bytecode *bck = calloc(len+1,sizeof(bytecode));
			bytecode *bcko = bck; 

			if(0==bck) {
				internalCompileError("Out of memory");
				YYERROR;
			}
			
			cur=$2->head;
			
			while(cur) {
				size_t curlen=bc_len(cur->obj);
				memcpy(bck, cur->obj, curlen*sizeof(bytecode));
				bck+=curlen;
				cur=cur->next;
			}


			bck->code = BC_END;
			
			// TODO: Check for sanity
			// TODO: Reduce bc_len calls
			// TODO: Check for correct order
			
			list_free($2);
			$$=bcko;
		}
		|
		stmt SEMI {
			$$=$1;
		};


// Parse direction currently goes backwards, 
stmts:
		stmts stmt SEMI {
			list_addToBack($1,$2);
			$$=$1;
		}
		|
		stmts control {
			list_addToBack($1,$2);
			$$=$1;
		}
		| 
		{
			$$=list_new();
		};
stmt:
		expression {
		  if($1 == (expression*)0) {
		    compileError("Invalid expression");
		    YYABORT;
		  }
			bytecode *code = expr_toBc($1);

			if(code==(bytecode*)0) {
			  compileError("Bad bytecode");
			  YYABORT;
			}
			int l = bc_len(code);
			DPRINTF("Statement of %i bytecodes\n",l);
			
			code = realloc(code, sizeof(bytecode)*(l+2));
			$1->isLiteral = true; // Don't free bytecode
			expr_free($1);

			code[l].code = BC_DPOP;
			code[l+1].code = BC_NONO;
			$$=code;
		}
        | 
        BREAK {
		  DPRINTF("Break statement\n");
		  bytecode *code = calloc(2, sizeof(bytecode));
		  code[0].code = BC_NOOP; //JMP 0 = break, tobe parsed in block
		  code[0].a = 1; 
		  code[1].code = BC_NONO;
		  $$=code;
        }
        |
        CONTINUE {
        	DPRINTF("Continue statement\n");
			bytecode *code = calloc(2, sizeof(bytecode));
			code[0].code = BC_NOOP; //NOOP 0 = continue, tobe parsed in block
			code[0].a = 2; 
			code[1].code = BC_NONO;        	
			$$=code;
        };
		
expression:
	OPENP expression CLOSEP {
	  $$ = $2;
	}
|
	math {
	}
|
	IDENTIFIER OPENP paramlist CLOSEP { 		// FUNCTION CALL
	  expression *ex = malloc(sizeof(expression));
	  bytecode *mcode;
	  ex->isFloat = false;
	  int lenBc, curBc;

	  bytecode callcode;
	  int numParams=0;
	  uint8_t *paramFlags = 0;
	  bool freeParamFlags = false;
	  bool found=false;

	  llist *curParam;	  

	  // PYES bytecode
	  if(strcmp($1,"yes")==0) {
	    callcode.code = BC_PYES;
	    callcode.a1 =1;
	    found = true;
	  } else if(strcmp($1,"no")==0) {
	    callcode.code = BC_PYES;
	    callcode.a1 =0;
	    found = true;
	  }
	  
	  // Built in funcs
	  if(!found) {
	    for(int i=0;i<builtins_len;i++) {
	      if(strcmp($1,builtins[i].name)==0) {
		DPRINTF("Function \"%s\" found is a built in function\n",$1);
		bool isFloat = builtins[i].isFloat;
		callcode.code = builtins[i].code;
		callcode.a1 =0;
		numParams = builtins[i].ac; 
		ex->isFloat = builtins[i].isFloat;
		paramFlags = malloc(sizeof(uint8_t)*numParams);
		freeParamFlags = true;
		for(int q=0;q<numParams;q++)
		  paramFlags[q] = ((isFloat)?FLOAT_VAR:0);
		found = true;
		break;
	      }
	    }
	  }
	  
	  // Native function
	  if(!found) {
	    symbolinfo symbol = searchSym($1,ccompart);
	    nativeFN *funk;
	    if(symbol.id<0) {
	      sprintf(sprt, "Function %s does not exist",$1);
	      compileError(sprt);
	      YYABORT;
	    } else if(symbol.isvar) {
	      sprintf(sprt,"Variable %s used as a function call",$1);
	      compileError(sprt);
	      YYABORT;
	    }
	    funk = &ccompart->vm->natives[symbol.id];
	    paramFlags = funk->paramFlags;
	    ex->isFloat = funk->retFloat;
	    numParams = funk->numParam;
	    callcode.code = BC_CALL;
	    callcode.a1 = symbol.id;
	    found = true;
	    
	    DPRINTF("Function \"%s\" found with id %i, function=%p\n",$1,symbol.id, funk->func);
	    assert(strcmp($1,funk->name)==0); 
	    assert(funk->func!=0);
	  }	    
	  
	  assert(found);
	  assert(paramFlags!=0 || numParams==0);
	  
	  
	  // Check no. arguments
	  if($3->length < numParams) {
	    sprintf(sprt, "Function %s has %i parameters, only %i found",$1, numParams, $3->length);
	    compileError(sprt);
	    YYABORT;
	  }

	  // TODO: make this the default check
	  if($3->length!=numParams)
	    DPRINTF("Warning: function called with too many parameters\n");

	  // 1 for callcode, 1 for BC_NONO
	  lenBc = 2;
	  
	  // Ready the parameters and count the size of the bytecodes
	  curParam = $3->head;
	  for(int i=0; i< numParams;i++) {
	    expression* ce = (expression*)curParam->obj;
	    autocast(paramFlags[i] & FLOAT_VAR, ce);
	    
	    if(paramFlags[i] & IS_ARRAY) {
	      sprintf(sprt, "Function %s has parameters %i as an array, however array wasn't found",$1, i);
	      compileError(sprt);
	      YYABORT;
	    }
	    
	    if(ce->isLiteral) {
	      ce->val.bcode = expr_toBc(ce);
	      ce->isLiteral = false;
	    }
	    lenBc+=bc_len(ce->val.bcode);
	    curParam = curParam->next;
	    DPRINTF("  Parameter %i has bytecode length %i, new total is %i\n",i, bc_len(ce->val.bcode), lenBc);
	  }

	  mcode = calloc(lenBc, sizeof(bytecode));
	  curBc = 0;

	  // Copy the parameters into mcode, last parameter first
	  for(int i=numParams-1;i>=0;i--) {
	    expression* ce = (expression*)list_get($3,i); // inneficient
	    int clen = bc_len(ce->val.bcode);
	    memcpy(&mcode[curBc],ce->val.bcode,sizeof(bytecode)*clen);
	    expr_free(ce);
	    curBc+=clen;
	  }

	  mcode[curBc] = callcode;
	  curBc++;
	  mcode[curBc].code = BC_NONO;
	  curBc++;
	  assert(curBc==lenBc);

	  ex->isLiteral = false;
	  ex->val.bcode = mcode;

	  if(freeParamFlags) free(paramFlags);
	  list_free($3);
	  // $3 contents already free'd in copy loop

	  DPRINTF("Function call completed with length %i with isFloat=%i\n", bc_len(ex->val.bcode), ex->isFloat);

	  $$ = ex;
	}
|
	retable {
	  $$ = $1;
	}
|
	IDENTIFIER ASSIGN expression {
	  int len;
	  
	  expression *ex = $3;

	  symbolinfo var = searchSym($1,ccompart);
	  
	  if(var.id<0) {
	    sprintf(sprt,"Symbol \"%s\" not resolved",$1);
	    compileError(sprt);
	    YYABORT;
	  }
	
	  if(!var.isvar) {
	    sprintf(sprt,"Symbol \"%s\" resolved to function, variable required", $1);
	    compileError(sprt);
	    YYABORT;
	  }

	  if(var.array) {
	    sprintf(sprt,"Symbol \"%s\" resolved to array, not yet implemented", $1);
	    compileError(sprt);
	    YYABORT;
	  }
	  DPRINTF("Assignent to variable \"%s\" with id=%i, local=%i, isfloat=%i\n", $1,var.id, var.local, var.isfloat);
	  autocast(var.isfloat,ex);
	  expr_ensureLit(ex);
	  len = bc_len(ex->val.bcode);
	  ex->val.bcode = realloc(ex->val.bcode, sizeof(bytecode)*(len+3));

	  ex->val.bcode[len].code = BC_DUP;
	  if(var.local)
	    ex->val.bcode[len+1].code = BC_POP;
	  else
	    ex->val.bcode[len+1].code = BC_GPOP;
	  ex->val.bcode[len+1].a1 = var.id;
	  ex->val.bcode[len+2].code = BC_NONO;

	  $$=ex;
	}
|
	cast expression { 
	  autocast($1==FLOAT,$2);
	  $$=$2;
	}
		
		
paramlist:
		moreparamlist {
			$$=$1
		}
		|
		{
			$$= list_new();
		}
		;

moreparamlist:	
		expression COMA moreparamlist {
			list_addToFront($3,$1);
			$$=$3;
		}
		| expression {
			list *lst = list_new();
			list_addToFront(lst,$1);
			$$=lst;
		}

		;

cast: 
		OPENP INT CLOSEP {
			$$ = INT; 
		}
		| 
		OPENP FLOAT CLOSEP {
			$$ = FLOAT;
		}
		;

		
////////////////////////////////////////////////////////////
// MATH		
////////////////////////////////////////////////////////////

%left         OR;
%left         AND;
%nonassoc     ISEQ ISNEQ;
%nonassoc     ISGEQ ISLEQ ISGT ISLT;
%left         PLUS MINUS;
%left         MULT DIV;
%left         NOT; //unary ops
//%left         OPENP; // ??

		
math:
		expression PLUS expression {
			$$=bin_op(PLUS,$1,$3);
		}
		| 
		expression MINUS expression{
			$$=bin_op(MINUS,$1,$3);
		}
		|
		expression MULT expression{
			$$=bin_op(MULT,$1,$3);
		}
		|
		expression DIV expression{
			$$=bin_op(DIV,$1,$3);
		}
		|
		expression OR expression{
			$$=bin_op(OR,$1,$3);
		}
		|
		expression AND expression{
			$$=bin_op(AND,$1,$3);
		}
		|
		expression ISEQ expression{
			$$=bin_op(ISEQ,$1,$3);
		}
		|
		expression ISNEQ expression{
			$$=bin_op(ISNEQ,$1,$3);
		}
		|
		expression ISGEQ expression{
			$$=bin_op(ISGEQ,$1,$3);
		}
		| 
		expression ISGT expression{
			$$=bin_op(ISGT,$1,$3);
		}
		|
		expression ISLT expression{
			$$=bin_op(ISLT,$1,$3);
		}
		| 
		NOT expression {
			//TODO: Not
			
		}
		|
		MINUS expression {
		  expression *e = calloc(1, sizeof(expression));
		  e->isLiteral = true;
		  e->isFloat = false;
		  e->val.in = 0;
		  $$=bin_op(MINUS,e,$2);
		}
		;
		
		
		
retable:
		IDENTIFIER {
		  symbolinfo var = searchSym($1,ccompart);
		  
		  if(var.id<0) {
		    sprintf(sprt,"Symbol \"%s\" not resolved",$1);
		    compileError(sprt);
		    YYABORT;
		  }
		  
		  if(!var.isvar) {
		    sprintf(sprt,"Symbol \"%s\" is a function but is used in an assignment",$1);
		    compileError(sprt);
		    YYABORT;
		  }
		  expression *ex = malloc(sizeof(expression));
		  bytecode *bc = malloc(2*sizeof(bytecode));
		  if(!var.local) {
		    bc->code = BC_GPSH;
		    bc->a1 = var.id;
		    }
		  else {
		    bc->code = BC_PUSH;
		    bc->a1 = var.id;
		  }
		  bc[1].code = BC_NONO;
		  ex->val.bcode=bc;
		  ex->isLiteral=false;
		  ex->isFloat=var.isfloat;
		  
		  $$ = ex;
		}
| 
		FLOAT_LIT { 
			expression *a = malloc(sizeof(expression));
			a->isFloat=true;
			a->isLiteral=true;
			a->val.fl=$1;
			$$ = a;
		} 
		| 
		INT_LIT { 
			expression *a = malloc(sizeof(expression));
			a->isFloat=false;
			a->isLiteral=true;
			a->val.in=$1;
			$$ = a;
		}
		;



control: 
		if
		|
		WHILE OPENP expression CLOSEP block {
  
		};

if: 
		IF OPENP expression CLOSEP block else {
		  // Plan: condition, cjmp on 0, block, jmp to end, else
		  int len, cpos;
		  bytecode *cond = expr_toBc($3);

		  int blockLen = bc_len($5);
		  int elseLen = (($6 !=(bytecode*)0)?bc_len($6):0);

		  assert(cond!=NULL);
		  if($3->isFloat)
		    compileWarning("Using float as boolean");
		  
		  len = bc_len(cond)+3+blockLen+1;
		  if(elseLen>0) {
		    len+=elseLen+1; // +1 for jmp instruction
		  }
		  cpos = 0;

		  cond = realloc(cond, sizeof(bytecode)*len);
		  cpos += bc_len(cond);
		  
		  cond[cpos].code = BC_CPUSH;
		  cond[cpos].a1 = 0;
		  cpos++;

		  cond[cpos].code = BC_CMP;
		  cpos++;

		  cond[cpos].code = BC_JMNE;
		  cond[cpos].sa = blockLen + ((elseLen>0)?2:0); // TODO: +1 since jump past jmp, why +2??
		  cpos++;

		  memcpy(&cond[cpos], $5, sizeof(bytecode)*blockLen);
		  cpos+=blockLen;
		  
		  if(elseLen>0) {
		    cond[cpos].code = BC_JMP;
		    cond[cpos].sa = elseLen;
		    cpos++;

		    memcpy(&cond[cpos], $6, sizeof(bytecode)*elseLen);
		    cpos+=elseLen;
		  }

		  cond[cpos].code = BC_NONO;
		  cpos++;

		  $3->isLiteral = true;

		  assert(cpos == len);

		  expr_free($3);
		  free($5);
		  if($6!=NULL) free($6);		  
		  $$ = cond;
		} 

else:
		ELSE if {
		  $$ = $2;
		}
		|
		ELSE block {
		  $$ = $2;
		}
		| 
		{
		  $$ = (bytecode*)0;
		}
		;
		

decls:
		decl SEMI decls {
		  DPRINTF("1 decl completed\n");
		  $$ = (node*)0;
		}
|
		{
		  $$ = (node*)0;
		};

decl:	
		global_modifier intfloat_keyword ident_list post_modifier {
		  bool isGlobal = $1;
		  bool isFloat = $2;
		  DPRINTF("Doing decl");
		  if(isGlobal) {
		    if(isFloat) {
		      char* iden;
		      while((iden = (char*)list_popFromFront($3))) {
			if(!vm_addFloat(ccompart->vm,iden)) {
			  sprintf(sprt, "Declaration of global float var %s failed",iden);
			  compileWarning(sprt);
			}
			
		      }
		    }
		    else {
		      char* iden;
		      while((iden = (char*)list_popFromFront($3))) {
			if(!vm_addInt(ccompart->vm,iden)) {
			  sprintf(sprt, "Declaration of global int var %s failed",iden);
			  compileWarning(sprt);
			}
		      }
		    }
		  }
		  else {
		    if(isFloat) {
		      char* iden;
		      while((iden = (char*)list_popFromFront($3))) {
			if(!com_addFloat(ccompart,iden)) {
			  sprintf(sprt, "Declaration of local float var %s failed",iden);
			  compileWarning(sprt);
			}
		      }
		    }
		    else {
		      char* iden;
		      while((iden = (char*)list_popFromFront($3))) {
			if(!com_addInt(ccompart,iden)) {
			  sprintf(sprt, "Declaration of local int var %s failed",iden);
			  compileWarning(sprt);
			}
		      }
		    }
		  }
		  DPRINTF(" - Done declare\n");
		  list_free($3);
			
		};
		
		
intfloat_keyword:
		INT  { 
		  DPRINTF("  Declaration is an int\n");
			$$=false; 
		}
		| 
		FLOAT { 
		  DPRINTF("  Declaration is a float\n");
			$$=true; 
		};
		
global_modifier:
		GLOBAL { 
		  DPRINTF("  Declaration is a global\n");
			$$=1; 
		}
		|  {
		  DPRINTF("  Declaration isn't a global\n");
			$$=0;
		};

post_modifier:
		OPENS expression CLOSES {
		  DPRINTF("  Declaration is array\n");
			if(!$2->isLiteral) {
				compileError("Array declaration with non-literal length");
				YYERROR;
			}
			autocast(false,$2);
			$$ = $2->val.in;
			if($$<0) {
				sprintf(sprt, "Array declared with length %i",$$);
				compileError(sprt);
				YYERROR;
			}
			free($2);
		}
		| 
		{
		  DPRINTF("  Declaration isn't array\n");
		  $$=-1;
		}

		
ident_list:
		more_ident_list {
		  DPRINTF("  Declaration of %i variables\n",$1->length);
		  $$=$1;
		}
		|
		{
		  DPRINTF("  Declaration of 0 variables\n");
			$$= list_new();
		}
		;

more_ident_list:	
		IDENTIFIER COMA more_ident_list {
			list_addToFront($3,$1);
			$$=$3;
		}
		| IDENTIFIER {
			list *lst = list_new();
			list_addToFront(lst,$1);
			$$=lst;
		}
		;		
				
%%
