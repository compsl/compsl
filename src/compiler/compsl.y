 	


%parse-param { char const *file_name };
%initial-action
{	
	//printf("Parsing file: %s",file_name);
};


%{
#include <stdio.h>
#include <string.h>
#include "node.h"
#include "../intern/gen.h"
#include <stdbool.h>

#define YYERROR_VERBOSE 1

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
  
//main() {
//        yyparse();
//        printf("Parsing successful\n");
//} 

%}

%union {
	int ival;
	float fval;
	char *sval;
	node *nval;
	bool bval;
	expression *expr;
}

%token <fval> FLOAT_LIT;
%token <ival> INT_LIT;
%token <sval> IDENTIFIER;
%token CUBBY GLOBAL DECLARE INT BOOL FLOAT TRUE FALSE IF ELSEIF ELSE WHILE BREAK RETURN SEMI COMA OPENB CLOSEB OPENP CLOSEP PLUS MINUS MULT DIV MOD ISEQ ISNEQ ASSIGN ISGEQ ISLEQ ISGT ISLT NOT AND OR;
%type <expr> expression math retable;
%type <bval> cast;
%type <nval> file header_stuff do_declare cubbys cubby block stmts stmt control else decls decl modifiers ident_list more_ident_list, paramlist;

%locations

%%
file:
		header_stuff do_declare cubbys
        ;

header_stuff: 
		;
		
do_declare:
		DECLARE OPENB decls CLOSEB;

cubbys:
		cubby cubbys | ;
cubby:
		CUBBY IDENTIFIER block {
			//printf("Cubby declared: %s\n",$2); 
		};
block:
		OPENB stmts CLOSEB | stmt SEMI;

stmts:
		stmt SEMI stmts 	|
		control stmts | ;
stmt:
		expression | RETURN | BREAK
		;
		
expression:
		OPENP expression CLOSEP {
			$$ = $2;
		}
		|
		math
		|
		IDENTIFIER OPENP paramlist CLOSEP { // FUNCTION CALL
			/*expression *ex = malloc(sizeof(expression));
			symbolinfo foo = searchSym($1,curCompart);
			if(!(foo.isvar)) {
				ex->isLiteral=false;
				nativeFN bar = comp->vm->natives[foo.id];
				//TODO get type from bar
				//TODO check var list
			}
			else {
				//TODO error here
			}*/
		}
		|
		retable
		|
		IDENTIFIER ASSIGN expression {
			//printf("Identifier: %s\n",$1);
		}
		|
		cast expression { 
			//if($1==CSL_FLOAT) {
				//if($2.isFloat) $$ = $2;
				//else {
					// TODO
				//}
			//}
			//else {
			//} 
		}
		;
		

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
			$$ = CSL_INT;
		}
		| 
		OPENP FLOAT CLOSEP {
			$$ = CSL_FLOAT;
		}
		;
		
		
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
			
		}
		| 
		expression MINUS expression
		|
		expression MULT expression
		|
		expression DIV expression
		|
		expression OR expression
		|
		expression AND expression
		|
		expression ISEQ expression
		|
		expression ISNEQ expression
		|
		expression ISGEQ expression
		| 
		expression ISGT expression
		|
		expression ISLT expression
		| 
		NOT expression
		
		;
		
		
		
retable:
		IDENTIFIER {
			
			//printf("Identifier: %s\n",$1);
		}
		| 
		FLOAT_LIT { //printf("%f\n",yylval.fval);
			} 
		| 
		INT_LIT { //printf("%i\n",yylval.ival); 
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
