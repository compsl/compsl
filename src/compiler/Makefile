
CC=gcc
# TODO: get CFLAGS in here, or move this file to main makefile. 

SRC=lex.yy.c compsl.tab.c 
OBJS=$(SRC:.c=.o)

all: lex.yy.c compsl.tab.c

#compiler: $(SRC) $(OBJS)
	
lex.yy.c: compsl.l 
	flex compsl.l 
	
compsl.tab.c: compsl.y compsl.l
	bison -d compsl.y

clean: 
	rm -f *.o compsl.tab.* lex.yy.c 
	
	 
#.c.o:
#	$(CC) -c $< -o $@
	
#test: compiler 
#	$(CC) *.o -o test