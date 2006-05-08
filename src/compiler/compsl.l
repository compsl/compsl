
%{
#include <stdio.h>
#include "node.h"
#include "compsl.tab.h"


%}
%%
   
\/\/.*\n		{}; /*comment*/
cubbyhole	return CUBBY;
global		return GLOBAL;
declare		return DECLARE;
int			return INT;
float		return FLOAT;
if			return IF;
elseif		return ELSEIF;
else		return ELSE;
while		return WHILE;
break		return BREAK;
return		return RETURN;
;			return SEMI;
,			return COMA;
\{			return OPENB; /*"bracket"*/
\}			return CLOSEB;
\(			return OPENP; /*"parenthesis"*/
\)			return CLOSEP;
\+			return PLUS;
-			return MINUS;
\*			return MULT;
\/			return DIV;
\%			return MOD;
==			return ISEQ;
!=			return ISNEQ;
=			return ASSIGN;
\>=			return ISGEQ;
\<=			return ISLEQ;
\>			return ISGT;
\<			return ISLT;
!			return NOT;
&&			return AND;
\|\|		return OR;
\ 			;
\t			;
\n			;
-?[0-9]+  	{	yylval.ival = atoi( yytext ); 
				return INT_LIT; 	
			}
-?[0-9]+\.[0-9]*  {	
	yylval.fval = atof(yytext);
	return FLOAT_LIT; 	
}
-?\.[0-9]+  {
	yylval.fval = atof(yytext);
	return FLOAT_LIT; 			
}
[_a-zA-Z0-9]+   {
	yylval.sval = malloc(sizeof(char)*(strlen(yytext)+1));
	if(yylval.sval==NULL) { 
		puts("Compiler out of memory\n");
		// TODO: Use parser's build in error fn 
		exit(1); 
	}
	strcpy(yylval.sval,yytext);
	return IDENTIFIER;
}

%%