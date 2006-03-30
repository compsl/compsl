
%{
#include <stdio.h>
#include "compsl.tab.h"
#define YYDEBUG 1 

%}
%%
\/\*.*\*\/		; /*comment*/
\/\/.*\n		; /*comment*/
-?[0-9]+  INT_LIT; 						/*PASS_ON*/
-?[0-9]+\.[0-9]*  return FLOAT_LIT; 	/*PASS_ON*/
-?\.[0-9]+  return FLOAT_LIT; 			/*PASS_ON*/
cubbyhole	return CUBBY;
global		return GLOBAL;
declare		return DECLARE;
int			return INT;
bool		return BOOL;
float		return FLOAT;
true		return TRUE;
false		return FALSE;
if			return IF;
elseif		return ELSEIF;
else		return ELSE;
while		return WHILE;
break		return BREAK;
return		return RETURN;
[a-zA-Z](_*[a-zA-Z0-9])*   return IDENTIFIER; 		/*PASS_ON*/
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

%%