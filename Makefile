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

#TODO: figure out how to make the test link against version of the lib with debug
#symbols but also have optimizations in the release worthy stuff ie: end up with two
# versions of the static lib, one with debug stuff and not optimization, and the other
# with no debug info and -O2

CC=gcc
CFLAGS=-c -ftabstop=4 -Wall -Wextra -Wfloat-equal -Wbad-function-cast -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-noreturn -Wunreachable-code -fPIC -std=gnu99 
# Any changes to CFLAGS probably need to be done to src/compiler/Makefile as well

ifdef DEBUG
	CFLAGS += -ggdb -D DEBUG
else
	CFLAGS += -O2 -fdata-sections -ffunction-sections -fbranch-target-load-optimize -frename-registers -fweb -ffast-math -fsingle-precision-constant -funroll-loops -finline-functions -fsched-spec-load -fsched2-use-superblocks -fmove-all-movables
	#TODO figure out if we need the -fno-strict-aliasing option.
endif

SOURCES=src/compartment.c  src/error.c  src/gen.c  src/run.c  src/vars.c  src/vm.c
OBJECTS=$(SOURCES:.c=.o)
TESTSRCS=src/test/test.c
TESTOBJS=$(TESTSRCS:.c=.o)

CMPLRPATH=src/compiler

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

compiler:
	make -C $(CMPLRPATH)


# INTERNAL TARGETS
.c.o:
	$(CC) $(CFLAGS) $< -o $@

$(STATIC_LIB_OUT):
	ar rcs $(STATIC_LIB_OUT) $(OBJECTS)

$(DYN_LIB_OUT): 
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.1 -o $(DYN_LIB_OUT)  $(OBJECTS)









