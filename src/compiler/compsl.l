
%{

// $Id$

/*
    CompSL scripting language 
    Copyright (C) 2007  Thomas Jones & John Peberdy

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "compiler/interncomp.h"
#include "compiler/node.h"
#include "compsl.h"
#include "intern/compartment.h"
#include "intern/gen.h"
#include "compsl.tab.h"

extern VM *cvm;
extern compart *ccompart;
int goparse(const char *fn, compart *com);
static void yyunput (int c,char *buf_ptr  ) __attribute__ ((unused));

int fileCompile(const char *filename ,compart* out) {
	FILE* input = fopen(filename, "r");
	int ret; 
	
	if(NULL == input) {
		DPRINTF("\n\n>> COULDN'T OPEN INPUT FILE - %s\n",filename);
		return -1;
	}
	yyin = input;
	yyrestart(yyin);
	
	ret = goparse(filename, out);
	fclose(input);
	return ret;
}
int stringCompile(const char *code, compart* out) {
	yy_scan_string(code);
	return goparse("STRING", out);
}  

%}
%option noyywrap
%%

\/\/.*\n		{}; /*comment*/
\/\* {
  register int c;

  for ( ; ; ) {
     while ( (c = input()) != '*' &&c != EOF ) ;
     if ( c == '*' ) {
       while ( (c = input()) == '*' ) ;
       if ( c == '/' ) break; 
     }
     if ( c == EOF ) {
       compileWarning( "Comment ended in EOF" );
       
       break;
     }
   }
}
declare	     return DECLARE_KEYWORD;
cubbyhole    return CUBBYHOLE_KEYWORD;
cubby    return CUBBY_KEYWORD;
global		return GLOBAL_KEYWORD;
int			return INT_KEYWORD;
float		return FLOAT_KEYWORD;
if			return IF_KEYWORD;
else		return ELSE_KEYWORD;
while		return WHILE_KEYWORD;
break		return BREAK_KEYWORD;
continue	return CONTINUE_KEYWORD;
;			return SEMICOLON;
,			return COMA;
\{			return OPEN_CURLY_BRACKET; /*"bracket"*/
\}			return CLOSE_CURLY_BRACKET;
\(			return OPEN_ROUND_BRACKET; /*"parenthesis"*/
\)			return CLOSE_ROUND_BRACKET;
\[			return OPEN_SQUARE_BRACKET;
\]			return CLOSE_SQUARE_BRACKET;
\+			return PLUS;
\+\+			return PLUSPLUS;
\+=			return PLUSEQ;
-			return MINUS;
--			return MINUSMINUS;
-=			return MINUSEQ;
\*			return MULT;
\*=			return MULTEQ;
\/			return DIV;
\/=			return DIVEQ;
\%			return MOD;
\%=			return MODEQ;
==			return ISEQ;
!=			return ISNEQ;
=			return ASSIGN;
\>=			return ISGEQ;
\<=			return ISLEQ;
\>			return ISGT;
\<			return ISLT;
!			return NOT;
\<\<			return SHIFT_LEFT;
\>\>			return SHIFT_RIGHT;
&&		return AND;
& 		return BITWISE_AND;
&=			return BITWISE_ANDEQ;
\|\|		return OR;
\|		return BITWISE_OR;
\|=			return BITWISE_OREQ;
\ 		      	;
\t			;
\n		{ COMPSL_CTRAC_PRNT("%4d\n",lineNo); lineNo++; };
[0-9]+  	{ yylval.ival = atoi( yytext ); return INT_LITERAL; }
[0-9]*\.[0-9]*  { yylval.fval = atof(yytext); return FLOAT_LITERAL; }
[_a-zA-Z0-9]+   {
	yylval.sval = calloc(strlen(yytext)+1, sizeof(char));
	if(yylval.sval==NULL) { 
		internalCompileError("Out of memory");
	}
	strcpy(yylval.sval,yytext);
	return IDENTIFIER;
}
<<EOF>> {yy_delete_buffer(YY_CURRENT_BUFFER); yyterminate();}
%%
