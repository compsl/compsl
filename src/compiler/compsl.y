%{
#include <stdio.h>
#include <string.h>

#define YYERROR_VERBOSE 1
 
 
void yyerror(const char *str)
{
		
        fprintf(stderr,"Error: %s\nLine Num: %i\n",str,12);
        
        exit(2);
}
 
int yywrap()
{
        return 1;
} 
  
main()
{
		
        yyparse();
        printf("Parsing successful\n");
} 

%}

%token INT_LIT FLOAT_LIT  FLOAT_LIT CUBBY GLOBAL DECLARE INT BOOL FLOAT TRUE FALSE IF ELSEIF ELSE WHILE BREAK RETURN IDENTIFIER SEMI COMA OPENB CLOSEB OPENP CLOSEP PLUS MINUS MULT DIV MOD ISEQ ISNEQ ASSIGN ISGEQ ISLEQ ISGT ISLT NOT AND OR;


%%
file:
		header_stuff do_declare cubbys
        ;

header_stuff: 
		;
		
do_declare:
		DECLARE block;

cubbys:
		cubby cubbys | ;
cubby:
		CUBBY IDENTIFIER block;
block:
		OPENB stmts CLOSEB | stmt SEMI;

stmts:
		stmt SEMI stmts 	|
		control stmts | ;
stmt:
		decl | expression | RETURN | BREAK
		;
		
expression:
		IDENTIFIER ASSIGN expression
		|
		OPENP expression CLOSEP;
		|
		expression PLUS expression
		|
		IDENTIFIER OPENP expression CLOSEP
		|
		IDENTIFIER OPENP CLOSEP
		|
		retable
		;
		
retable:
		IDENTIFIER | FLOAT_LIT | INT_LIT
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