 	


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
//#include "../intern/debug.h"

#define YYERROR_VERBOSE 1

// NOTE: ANY INCLUDES HERE SHOULD ALSO GO IN compsl.l probably


extern FILE *yyin;
extern compart *ccompart;
char *sprt;

int goparse(char* fn, compart *com) {
	DPRINTF("\n\n>> STARTING PARSE - %s\n",fn);
	ccompart = com;
	yyin = fopen( fn, "r" );
	yyrestart(yyin);
	sprt=malloc(1024 * sizeof(char));
	int ret = yyparse(fn);
	DPRINTF(">> DONE PARSE\n\n");
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
	fprintf(stderr,"Compile error: %s\n",str);
	//	exit(1);
	//YYABORT; 
	//TODO: make friendly
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
%type <nval> file header_stuff do_declare cubbys cubby control else decl modifiers;
%type <sval> cubby_id;
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
			int l = bc_len(code);
			DPRINTF("Statement of %i bytecodes\n",l);
			code = realloc(code, sizeof(bytecode)*(l+2));
			code[l].code = BC_DPOP;
			code[l+1].code = BC_NONO;
			$$=code;
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
		math
		|
		IDENTIFIER OPENP paramlist CLOSEP { 		// FUNCTION CALL
			expression *ex = malloc(sizeof(expression));
			
			//Special cases for debugging
			if($1[0]=='y'&& $1[1]=='e' && $1[2]=='s' && $1[3]==0) {
				bytecode *mcode = malloc(2*sizeof(bytecode));
				mcode[0].code = BC_PYES;
				mcode[0].a1 =1;
				mcode[1].code=BC_NONO;
				ex->val.bcode=mcode;
			}
			else if($1[0]=='n'&& $1[1]=='o' && $1[3]==0) {
				bytecode *mcode = malloc(2*sizeof(bytecode));
				mcode[0].code = BC_PYES;
				mcode[0].a1 =0;
				mcode[1].code=BC_NONO;
				ex->val.bcode=mcode;			
			}
			
			// Its a built in function call
			else {
				symbolinfo foo = searchSym($1,ccompart);
				
				//TODO: what does searchSym return for not found?
				if(foo.id!=0 && foo.id!=-1 && !foo.isvar) {
					DPRINTF("Functions \"%s\" found with id %i\n",$1,foo.id);
					ex->isLiteral=false;
					nativeFN funk = ccompart->vm->natives[foo.id];
					//TODO: get type from func
					//TODO: check var list
					
				}
				else if(foo.isvar) {
					sprintf(sprt,"Variable %s used as a function call",$1);
					compileError(sprt);
					YYERROR;
				}
				else {
					DPRINTF("Functions \"%s\" not found with id %i\n",$1,foo.id);
					
					bytecode *mcode = malloc(2*sizeof(bytecode));
					mcode[0].code = BC_PYES;
					mcode[0].a1 =0;
					mcode[1].code=BC_NONO;
					ex->val.bcode=mcode;			
					
					//TODO: remove comments below and remove code above
					
					//sprintf("Function %s does not exist",$1);
					//compileError(sprt);
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
