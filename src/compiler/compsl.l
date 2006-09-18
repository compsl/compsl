
%{
#include <stdio.h>
#include <stdbool.h>
#include "node.h"
#include "../extern/vm.h"
#include "../extern/compart.h"
#include "../intern/gen.h"
#include "compsl.tab.h"

extern VM *cvm;
extern compart *ccompart;

%}
%%
   
\/\/.*\n		{}; /*comment*/
declare		{
	return DECLARE;
}
cubbyhole	{
	return CUBBY;
}
global		return GLOBAL;
int			return INT;
float		return FLOAT;
if			return IF;
elseif		return ELSEIF;
else		return ELSE;
while		return WHILE;
break		return BREAK;
return		return RETURN;
continue	return CONTINUE;
;			return SEMI;
,			return COMA;
\{			return OPENB; /*"bracket"*/
\}			return CLOSEB;
\(			return OPENP; /*"parenthesis"*/
\)			return CLOSEP;
\[			return OPENS;
\]			return CLOSES;
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
		internalCompileError("Out of memory");
	}
	strcpy(yylval.sval,yytext);
	return IDENTIFIER;
}

%%