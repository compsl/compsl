 	


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
#include "interncomp.h"
#include "compsl.tab.h"
#include "../intern/gen.h"
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../intern/compartment.h"
#include "../intern/builtins.h"
#include "../extern/compsl.h"
#include "../extern/port.h"

#define YYERROR_VERBOSE 1
  
  // NOTE: ANY INCLUDES HERE SHOULD ALSO GO IN compsl.l probably  
  
  extern FILE *yyin;
  
  int goparse(FILE* input,const char *fn, compart *com) {
    lineNo=1;
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
    free(sprt); sprt = NULL;
    return ret;
  }
  int fileCompile(const char *filename , VM* vm, compart* out) {
	FILE* input = fopen(filename, "r");
    return goparse(input,filename, out);
  }
  int stringCompile(const char *code, VM* vm, compart* out) {
    FILE* input = fmemopen(code,strlen(code)*sizeof(char),"r");
    return goparse(input,"STRING", out);
  }
  
void yyerror(const char *fn, const char *msg) {
    fprintf(stderr,"> In file \"%s\"\n  Error: \"%s\"\n  Line Num: %i\n",fn,msg,lineNo);
    return; 
}

int yywrap(void) {
        return 1;
} 


  

///////////////////////////////////////////////////////////////////////
// END FUNC DECLS

%}

%union {
	int ival;
  	float fval;
	char *sval;
	bool bval;
	expression *expr;
	list *xlist;
	bytecode *bc;
}

%token <fval> FLOAT_LIT;
%token <ival> INT_LIT; 
%token <sval> IDENTIFIER;
%token CUBBY GLOBAL INT BOOL FLOAT TRUE FALSE IF ELSEIF ELSE WHILE BREAK RETURN SEMI COMA OPENB CLOSEB OPENP CLOSEP ISEQ ISNEQ ASSIGN ISGEQ ISLEQ ISGT ISLT NOT AND OR DECLARE PLUS MINUS MULT DIV MOD CLOSES OPENS CONTINUE BOR BAND POW SHFTL SHFTR; 
%type <expr> expression math retable;
%type <ival> cast post_modifier;
%type <sval> cubby_id;
%type <bval> global_modifier intfloat_keyword;
%type <bc> stmt block ife elsee control;
%type <xlist> paramlist stmts moreparamlist ident_list more_ident_list;

%type <void> file header_stuff do_declare cubbys cubby decl decls;

%locations

%%
file:
		header_stuff do_declare cubbys
        ;

header_stuff: {};
		
do_declare: {
		DPRINTF("Doing declare\n");
		}
		DECLARE OPENB decls CLOSEB {
		  DPRINTF("Done declare\n");
		}

cubbys:
		cubby cubbys {} | {};
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
			DPRINTF("Parsed block with %i statements\n",$2->length);

			llist *cur=$2->head;
			bytecode *mcode;
			int len=0, cpos=0;
			
			// Get the length
			while($2->length>0 && cur) {
			  assert(cur->obj!=NULL); 
			  len+=bc_len(cur->obj);
			  cur=cur->next;
			}
			len+=1; // for the BC_END
			
			mcode = calloc(len+1,sizeof(bytecode)); 
			assert(mcode!=NULL);
			
			cur=$2->head;
			
			// Copy in
			while($2->length>0 && cur) {
				int curlen=bc_len(cur->obj);
				memcpy(&mcode[cpos], cur->obj, curlen*sizeof(bytecode));
				free(cur->obj); cur->obj = NULL;
				cpos+=curlen;
				cur=cur->next;
			}

			// Terminate
			mcode[cpos].code = BC_END;
			cpos++;

			assert(cpos==len);
			list_free($2);
			$$=mcode;
		}
		|
		stmt SEMI {
			$$=$1;
		}
		|
		SEMI {
		  bytecode *r = calloc(1,sizeof(bytecode));
		  r[0].code=BC_NONO;
		  $$ = r;
		};


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
		  // This is before the first element of the stmts list		     
			$$=list_new();
		};
stmt:
		expression {
		  if($1 == NULL) {
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
			if(code == NULL) internalCompileError("Out of Memory");
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
		  code[0].a = BREAK_NOOP_IDEN; 
		  code[1].code = BC_NONO;
		  $$=code;
        }
        |
        CONTINUE {
        	DPRINTF("Continue statement\n");
			bytecode *code = calloc(2, sizeof(bytecode));
			code[0].code = BC_NOOP; //NOOP 0 = continue, tobe parsed in block
			code[0].a = CONTINUE_NOOP_IDEN; 
			code[1].code = BC_NONO;        	
			$$=code;
        };
		
expression:
	OPENP expression CLOSEP {
	  $$ = $2;
	}
|
	math {
	  $$ = $1;
	}
|
	IDENTIFIER OPENP paramlist CLOSEP { 		// FUNCTION CALL
	  $$ = function_call($1, $3);
	  if(NULL == $$) 
	    YYABORT;
	}
|
	retable {
	  $$ = $1;
	}
|
	IDENTIFIER ASSIGN expression {
	  $$ = assignVar($1, $3);
	  if(NULL == $$) 
	    YYABORT;
	}
|
	cast expression { 
	  expr_autocast($1==FLOAT,$2);
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

%nonassoc     ISEQ ISNEQ;
%nonassoc     ISGEQ ISLEQ ISGT ISLT;
%left         OR;
%left         AND;
%left         PLUS MINUS;
%left         MULT DIV;
%left         NOT; //unary ops
		

math:
expression PLUS expression {   $$=bin_op(PLUS,$1,$3);}
|
expression MINUS expression {   $$=bin_op(MINUS,$1,$3);}
|
expression MULT expression {   $$=bin_op(MULT,$1,$3);}
|
expression DIV expression {   $$=bin_op(DIV,$1,$3);}
|
expression MOD expression {   $$=bin_op(MOD,$1,$3);}
|
expression OR expression {   $$=bin_op(OR,$1,$3);}
|
expression AND expression {   $$=bin_op(AND,$1,$3);}
|
expression ISEQ expression {   $$=bin_op(ISEQ,$1,$3);}
|
expression ISNEQ expression {   $$=bin_op(ISNEQ,$1,$3);}
|
expression ISGEQ expression {   $$=bin_op(ISGEQ,$1,$3);}
|
expression ISGT expression {   $$=bin_op(ISGT,$1,$3);}
|
expression ISLEQ expression {   $$=bin_op(ISLEQ,$1,$3);}
|
expression ISLT expression {   $$=bin_op(ISLT,$1,$3);}
|
NOT expression {
  expr_ensureLit($2);
    
  int len = bc_len($2->val.bcode);
  $2->val.bcode = realloc($2->val.bcode, (len+2)*sizeof(bytecode));
  $2->val.bcode[len].code = BC_NOT;
  $2->val.bcode[len+1].code = BC_NONO;
  $$ = $2;
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
		  $$ = readVar($1);
		  if(NULL == $$) {
		    YYABORT;
		  }
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
		ife
		|
		WHILE OPENP expression CLOSEP block {
		  $$ = ctrlWhile($3,$5);
		  if(NULL == $$) {
		    YYABORT;
		  }
		};

ife: 
		IF OPENP expression CLOSEP block elsee {
		  $$ = ctrlIf($3,$5,$6);
		  if(NULL == $$) {
		    YYABORT;
		  }
		} 

elsee:
		ELSE ife { $$ = $2; }
		|
		ELSE block { $$ = $2; }
		| 
		{ $$ = (bytecode*)0; }
		;
		
decls:
		decl SEMI decls {} | {};

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
			expr_autocast(false,$2);
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
