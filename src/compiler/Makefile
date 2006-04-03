
all: lex.yy.c compsl.tab.c
	
lex.yy.c: compsl.l 
	flex compsl.l 
	
compsl.tab.c: compsl.y compsl.l
	bison -d compsl.y

clean: 
	rm -f *.o compsl.tab.* lex.yy.c 
	
	 