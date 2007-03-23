 	


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
  ASSERT((a!=(expression*)0) && (b!=(expression*)0),"Bad args for bin_bc_op");
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
    
  default:
    sprintf(sprt,"Operation \"%i\" not implemented\n",op);
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
  ASSERT((a!=(expression*)0) && (b!=(expression*)0),"Bad args for bin_op");
	if(a->isLiteral && b->isLiteral) {
		return bin_lit_op(op,a,b);
	}
	else {
		return bin_bc_op(op,a,b);
	}
}


// More helpers

expression* readVar(char* name) {
	symbolinfo foo = searchSym(name,ccompart);

	if(foo.id==-1) {
		sprintf(sprt,"Symbol \"%s\" not resolved\nexiting.\n",name);
		compileError(sprt);
		return 0;
	}
	
	if(foo.isvar) {
		expression *ex = malloc(sizeof(expression));
		bytecode *bc = malloc(2*sizeof(bytecode));
		if(!foo.local) {
			bc->code = BC_GPSH;
			// TODO: args
		}
		else {
			bc->code = BC_PUSH;
			// TODO: args
		}
		bc[1].code = BC_NONO;
		ex->val.bcode=bc;
		ex->isLiteral=false;
		ex->isFloat=foo.isfloat;
		
		return ex;
	}
	return 0;
}

void compileError(const char *str) {
	fprintf(stderr,"Compile error: %s, ---ABORTING---\n",str);
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
%type <ival> cast post_modifier intfloat_keyword;
%type <nval> file header_stuff do_declare cubbys cubby control else decl;
%type <sval> cubby_id;
%type <bval> modifiers;
%type <bc> stmt block;
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
			while(cur) {
				len+=bc_len(cur->obj);
				cur=cur->next;
			}

			
			bytecode *bck = malloc((len+1)*sizeof(bytecode));
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


/*TODO: WHICH WAY IS PARSING GOING? =+++++++!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!+++++++++++++++++*/
stmts:
		stmt SEMI stmts {
			list_addToFront($3,$1);
			$$=$3;
		}
		|
		control stmts {
			list_addToFront($2,$1);
			$$=$2;
		}
		| 
		{
			$$=list_new();
		};
stmt:
		expression {
			bytecode *code = expr_toBc($1);
			bytecode *ncode;

			if(code==(bytecode*)0) {
			  compileError("Bad bytecode");
			  YYABORT;
			}
			int l = bc_len(code);
			DPRINTF("Statement of %i bytecodes\n",l);
			
			ncode = malloc(sizeof(bytecode) * (l+2));

			ASSERT(ncode!=(void*)0,"Out of memory");

			memcpy(ncode, code, sizeof(bytecode)*l);
			
			// TODO: realloc borks !!
			//code = realloc(code, sizeof(bytecode)*(l+2));
			expr_free($1);

			ncode[l].code = BC_DPOP;
			ncode[l+1].code = BC_NONO;
			$$=ncode;
		}
        | 
        BREAK {
		  DPRINTF("Break statement\n");
		  bytecode *code = malloc(sizeof(bytecode)*2);
		  code[0].code = BC_JMP; //JMP 0 = break, tobe parsed in block
		  code[0].a = 0; 
		  code[1].code = BC_NONO;
		  $$=code;
        }
        |
        CONTINUE {
        	DPRINTF("Continue statement\n");
			bytecode *code = malloc(sizeof(bytecode)*2);
			code[0].code = BC_NOOP; //NOOP 0 = continue, tobe parsed in block
			code[1].code = BC_NONO;        	
			$$=code;
        }
		;
		
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
	  int lenBc, curBc = 0;

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
	    ASSERT(strcmp($1,funk->name)==0, "Wrong function found");
	    ASSERT(funk->func!=0, "Unitialized function called");
	  }	    
	  
	  ASSERT(found, "Function not found but not aborted");
	  ASSERT(paramFlags!=0 || numParams==0, "No paramater specs");
	  
	  
	  // Check no. arguments
	  if($3->length < numParams) {
	    sprintf(sprt, "Function %s has %i parameters, %i found",$1, numParams, $3->length);
	    compileError(sprt);
	    YYABORT;
	  }
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

	  mcode = malloc(sizeof(bytecode)+lenBc);

	  // Copy the parameters into mcode
	  curParam = $3->head;
	  for(int i=0;i < numParams;i++) {
	    expression* ce = (expression*)curParam->obj;
	    int clen = bc_len(ce->val.bcode);
	    memcpy(&mcode[curBc],ce->val.bcode,sizeof(bytecode)*clen);
	    curBc+=clen;
	    curParam = curParam->next;
	  }

	  mcode[curBc] = callcode;
	  curBc++;
	  mcode[curBc].code = BC_NONO;
	  curBc++;
	  ASSERT(curBc==lenBc, "Logic error adding parameters to function call");	    

	  ex->isLiteral = false;
	  ex->val.bcode = mcode;

	  if(freeParamFlags) free(paramFlags);
	  list_free($3);

	  DPRINTF("Function call completed with length %i with isFloat=%i\n", bc_len(ex->val.bcode), ex->isFloat);

	  $$ = ex;
	}
|
	retable {
	  $$ = $1;
	}
|
	IDENTIFIER ASSIGN expression {
	  //expression *lhs = resolveVar($1);
	  //autocast(lhs->isFloat,$3);
	  
	  //TODO: here			
	}
|
	cast expression { 
	  autocast($1==FLOAT,$2);
	  $$=$2;
	}
		
		
/*TODO: direction*/
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
		;
		
		
		
retable:
		IDENTIFIER {
			$$=readVar($1);
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
		IF OPENP expression CLOSEP block else
		|
		WHILE OPENP expression CLOSEP block
		;

else:
		ELSEIF OPENP expression CLOSEP block else
		|
		ELSE block
		|
		;
		

decls:
		decl SEMI decls | ;
			
decl:	
		modifiers intfloat_keyword ident_list post_modifier {
			if($1) {
				if($2) {
					char* iden;
					while((iden = (char*)list_popFromFront($3))) {
						vm_addFloat(ccompart->vm,iden);
						free(iden);
					}
				}
				else {
					char* iden;
					while((iden = (char*)list_popFromFront($3))) {
						vm_addInt(ccompart->vm,iden);
						free(iden);
					}
				}
			}
			else {
				if($2) {
					char* iden;
					while((iden = (char*)list_popFromFront($3))) {
						com_addFloat(ccompart,iden);
						free(iden);
					}
				}
				else {
					char* iden;
					while((iden = (char*)list_popFromFront($3))) {
						com_addInt(ccompart,iden);
						free(iden);
					}
				}
			}
			free($3);
		};
		
		
intfloat_keyword:
		INT  { 
			$$=0; 
		}
		| 
		FLOAT { 
			$$=1; 
		};
		
modifiers:
		GLOBAL { 
			$$=1; 
		}
		|  {
			$$=0;
		};

post_modifier:
		OPENS expression CLOSES {
			if(!$2->isLiteral) {
				compileError("Array declaration with non-literal length\n");
				YYERROR;
			}
			autocast(false,$2);
			$$ = $2->val.in;
			if($$<0) {
				sprintf(sprt, "Array declared with length %i\n",$$);
				compileError(sprt);
				YYERROR;
			}
			free($2);
		}
		| 
		{
			$$=-1;
		}

		
ident_list:
		more_ident_list {
			$$=$1
		}
		|
		{
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
