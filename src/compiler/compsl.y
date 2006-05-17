 	


%parse-param { char const *file_name };
%initial-action
{	
	//printf("Parsing file: %s",file_name);
};


%{
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "node.h"
#include "../intern/gen.h"
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../intern/bytecode.h"

#define YYERROR_VERBOSE 1

// NOTE: ANY INCLUDES HERE SHOULD ALSO GO IN compsl.l probably


extern FILE *yyin;

int goparse(char* fn) {
	yyin = fopen( fn, "r" );
	yyrestart(yyin);
	return yyparse(fn);
}

void yyerror(const char *fn, const char *msg) {
    fprintf(stderr,"> In file \"%s\"\n  Error: \"%s\"\n  Line Num: %i\n",fn,msg,-1);//yylloc.first_line);
	exit(2);
}

int yywrap() {
        return 1;
} 

////////////////////////////////////////////////////////////////
//Variables
////////////////////////////////////////////////////////////////


  


////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////

// Binary operator helpers for bin_op()

expression* bin_bc_op(int bc,expression* a, expression* b) {
	return a;
	//TODO: this
}


expression* bin_lit_op(int op, expression* a, expression* b) {
	if(!a->isFloat && !b->isFloat) {
		if(op==PLUS)
			a->val.in+=b->val.in;
		else if(op==MINUS)
			a->val.in-=b->val.in;
		else {
			puts("Unknown op");
			exit(1); //TODO: this
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
	if(a->isLiteral && b->isLiteral) {
		return bin_lit_op(op,a,b);
	}
	else {
		return bin_bc_op(op,a,b);
	}
}


// More helpers

expression* resolveVar(char* name) {
	symbolinfo foo = searchSym(name,ccompart);

	if(foo.id!=0 && foo.isvar) {
		expression *ex = malloc(sizeof(expression));
		ex->isLiteral=false;
		ex->isFloat=foo.isfloat;
		return ex;
	}
	//TODO: resolveVar
	return 0;
}

void autocast(bool toFloat,expression *e) {
	if(e->isLiteral) {
		if( toFloat && !e->isFloat) {
			e->val.fl = (float)e->val.in;
			e->isFloat=true;
		}
		else if(!toFloat && e->isFloat) {
			e->val.in = (int)e->val.fl;
			e->isFloat=false;
		} 
	}
	else {
		puts("Casting not fully implemented yet");
	}
}


int bc_len(bytecode* bc) {
	int len=0;
	while(bc->code) {
		bc++;
		len++;
	} 
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
%token CUBBY GLOBAL INT BOOL FLOAT TRUE FALSE IF ELSEIF ELSE WHILE BREAK RETURN SEMI COMA OPENB CLOSEB OPENP CLOSEP ISEQ ISNEQ ASSIGN ISGEQ ISLEQ ISGT ISLT NOT AND OR DECLARE PLUS MINUS MULT DIV MOD;
%type <expr> expression math retable;
%type <ival> cast;
%type <nval> file header_stuff do_declare cubbys cubby block stmts control else decls decl modifiers ident_list more_ident_list;
%type <bc> stmt;
%type <xlist> paramlist;

%locations

%%
file:
		header_stuff do_declare cubbys
        ;

header_stuff: 
		;
		
do_declare:
		DECLARE OPENB decls CLOSEB {
			
		}

cubbys:
		cubby cubbys | ;
cubby:
		CUBBY IDENTIFIER block {
			
			//printf("Cubby declared: %s\n",$2); 
			
			
		}
block:
		OPENB stmts CLOSEB
		|
		stmt SEMI;

stmts:
		stmt SEMI stmts{
			
		}
		|
		control stmts | ;
stmt:
		expression {
			bytecode *code = expr_toBc($1);
			int l = bc_len(code);
			
			bytecode *newCode=malloc(sizeof(bytecode)*(l+2));
			bytecode *t = newCode;
			
			while( code->code ) { 
				*t = *code; 
				t++; 
				code++; 
			}
			t->code = BC_POP;
			t++;
			t=0;
			$$=newCode;
			
			//TODO check this
		}
		|
		RETURN | BREAK
		;
		
expression:
		OPENP expression CLOSEP {
			$$ = $2;
		}
		|
		math
		|
		IDENTIFIER OPENP paramlist CLOSEP { 		// FUNCTION CALL
			expression *ex = malloc(sizeof(expression));
			
			//Special case for debugging
			if($1[0]=='y'&& $1[1]=='e' && $1[2]=='s' && $1[3]==0) {
				bytecode *mcode = malloc(sizeof(bytecode));
				mcode->code = BC_PYES;
				mcode->a1 =1;
				ex->val.bcode=mcode;
			}
			else {
				symbolinfo foo = searchSym($1,ccompart);
				if(foo.id!=0 && !foo.isvar) {
					ex->isLiteral=false;
					nativeFN funk = ccompart->vm->natives[foo.id];
					//TODO: get type from func
					//TODO: check var list
				}
				else {
					printf("Function %s does not exist",$1);
					
				}
			}
			list_free($3);
			$$ = ex;
		}
		|
		retable {
			$$ = $1;
		}
		|
		IDENTIFIER ASSIGN expression {
			expression *lhs = resolveVar($1);
			autocast(lhs->isFloat,$3);
			
			//TODO: here			
		}
		|
		cast expression { 
			autocast($1==FLOAT,$2);
			$$=$2;
		}
		
		

paramlist:
		expression moreparamlist
		|
		;
		
moreparamlist:
		COMA expression  moreparamlist
		|
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
			expression *a = malloc(sizeof(expression));
			a->isLiteral=false;
			symbolinfo si = searchSym($1,ccompart);
			if(si.id==0) {
				printf("The variable \"%s\" is used but not declared",$1);
			}
			else {
				a->isFloat=si.isfloat;
			}
			// TODO set bytecode 
			$$=a;
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
		modifiers INT ident_list
		|
		modifiers FLOAT ident_list 
		;
		
modifiers:
		GLOBAL | ;

		
ident_list:
		IDENTIFIER more_ident_list;
more_ident_list:
		COMA IDENTIFIER more_ident_list
		|
		;		
				
%%
