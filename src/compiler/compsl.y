%{
#include <stdio.h>
#include <string.h>
#define YYDEBUG 1 
 
void yyerror(const char *str)
{
        fprintf(stderr,"Error: %s\n",str);
        exit(2);
}
 
int yywrap()
{
        return 1;
} 
  
main()
{
		printf("CompSL is parsing..");
        yyparse();
        printf("CompSL is done");
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
		cubbys cubby | ;
cubby:
		CUBBY IDENTIFIER block;
block:
		OPENB stmts CLOSEB;

stmts:
		stmt SEMI stmts 	|;
stmt:
		control | decl | expression ;
		
expression:
		IDENTIFIER ASSIGN expression
		|
		OPENP expression CLOSEP;
		|
		expression PLUS expression
		|
		IDENTIFIER OPENP stmt CLOSEP
		;

control: 
		IF OPENB stmts CLOSEB else
		|
		WHILE OPENP expression CLOSEP OPENB stmts CLOSEP
		;

else:
		ELSEIF block else
		|
		ELSE block
		|
		;
		

		
decl:	
		modifiers INT ident_list SEMI
		|
		modifiers FLOAT ident_list SEMI
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