#
#John's hackish general purpose makefile hacked up - 17Mar06
#
#
# Build compsl into a library 
#
#
#Links
#=========================================================================
# http://www.cs.washington.edu/orgs/acm/tutorials/dev-in-unix/compiler.html
# http://www.gnu.org/fun/jokes/helloworld.html
# http://www.adp-gmbh.ch/cpp/gcc/create_lib.html
# http://www.eng.hawaii.edu/Tutor/Make/3-1.html



CC=gcc
CFLAGS=-c -Wall -fPIC -std=gnu99 

SOURCES=src/compartment.c  src/error.c  src/gen.c  src/run.c  src/vars.c  src/vm.c
OBJECTS=$(SOURCES:.c=.o)
TESTSRCS=src/test/test.c
TESTOBJS=$(TESTSRCS:.c=.o)

SHORTLIB=compsl
LIBNAME=lib$(SHORTLIB)

TEST_EXE=bin/compsl-test
STATIC_LIB_OUT=bin/$(LIBNAME).a
DYN_LIB_OUT=bin/$(LIBNAME).so.1.0.1

#TARGETS
all: compile static dynamic maketestonly
	         
compile: $(SOURCES) $(OBJECTS)

static: compile $(STATIC_LIB_OUT)
dynamic: compile $(DYN_LIB_OUT)

test: maketestonly
	$(TEST_EXE)

maketestonly: $(TESTSRCS) $(TESTOBJS) static
	$(CC) -static $(TESTSRCS) -Lbin -l$(SHORTLIB) -o $(TEST_EXE)

clean:
	rm -f $(OBJECTS) $(STATIC_LIB_OUT) $(DYN_LIB_OUT) $(TEST_EXE)




# INTERNAL TARGETS
.c.o:
	$(CC) $(CFLAGS) $< -o $@

$(STATIC_LIB_OUT):
	ar rcs $(STATIC_LIB_OUT) $(OBJECTS)

$(DYN_LIB_OUT): 
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.1 -o $(DYN_LIB_OUT)  $(OBJECTS)









