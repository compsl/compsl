#
#John's hackish general purpose makefile hacked up - 17Mar06
#
#
# Build compsl into a library 
#
#
#Links
# http://www.adp-gmbh.ch/cpp/gcc/create_lib.html

#TODO: figure out how to make the test link against version of the lib with debug
# symbols but also have optimizations in the release worthy stuff ie: end up with two
# versions of the static lib, one with debug stuff and not optimization, and the other
# with no debug info and -O2

DEBUG=1

CC=gcc
CFLAGS=-ftabstop=4 -Wall -Wextra -Wfloat-equal -Wbad-function-cast -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-noreturn -Wunreachable-code -std=gnu99 -ffast-math -fno-math-errno -funsafe-math-optimizations -fno-trapping-math
# if none x86 need to disable this line
CFLAGS += -mmmx -mno-ieee-fp
#if sse instructions not available need to disable this line
CFLAGS += -msse -mfpmath=sse

ifdef DEBUG
	CFLAGS += -O0 -ggdb -D DEBUG
else
	#CFLAGS += -O2
	CFLAGS += -O2 -fdata-sections -ffunction-sections -fbranch-target-load-optimize -frename-registers -fweb -fsingle-precision-constant -funroll-loops -finline-functions -fsched-spec-load -fsched2-use-superblocks -fmove-all-movables
	# TODO: figure out if we need the -fno-strict-aliasing option.
	# TODO: make sure none of these breaks the library for linking....
endif

CMPLRPATH=src/compiler
SHORTLIB=compsl
LIBNAME := lib$(SHORTLIB)

#All sources except test and flex/bison generated sources
REG_SRCS=src/compartment.c  src/error.c  src/gen.c  src/run.c  src/vars.c src/vm.c src/compiler/comp.c

#Derived srcs
FB_COMPILER_SRCS=$(CMPLRPATH)/lex.yy.c $(CMPLRPATH)/compsl.tab.c

SOURCES :=  $(REG_SRCS) $(FB_COMPILER_SRCS)
OBJECTS := $(SOURCES:.c=.o)
#DEPS := $(SOURCES:.c=.d)


TESTSRCS := $(addprefix src/test/,test-interp.c test-comp.c test-api.c)
TESTOBJS := $(TESTSRCS:.c=.o)
TEST_EXES := $(addprefix bin/,$(notdir $(basename $(TESTSRCS))))

STATIC_LIB_OUT := bin/$(LIBNAME).a
DYN_LIB_OUT := bin/$(LIBNAME).so.1.0.1

#TARGETS
all:  common static dynamic maketestonly

common: derived depend compile


depend:
	makedepend -fDependfile -- $(CFLAGS) -- $(SOURCES)

derived: $(FB_COMPILER_SRCS)

clean:
	rm -f $(OBJECTS) $(TESTOBJS) $(STATIC_LIB_OUT) $(DYN_LIB_OUT) $(CMPRL_TEST_EXE) $(DEPS) $(TEST_EXES:=*) src/compiler/compsl.tab.h src/compiler/compsl.tab.c src/compiler/lex.yy.c
	         
compile: $(SOURCES) $(OBJECTS)

static: common $(STATIC_LIB_OUT)
dynamic: common $(DYN_LIB_OUT)

cleantest: 
	make clean 
	make test

test: maketestonly
	for test in  $(TEST_EXES); do \
		$$test; \
	done
	
# make test executibles, assumes that all tests are single object/source file
# linked to libcompsl.a
maketestonly: $(TESTOBJS) static
	for obj in $(TESTOBJS); do \
		$(CC) -static $$obj -Lbin -l$(SHORTLIB) -o bin/$$(basename $$obj .o); \
	done


# INTERNAL TARGETS

include Dependfile

#$(DEPS): $(SOURCES)
#	$(CC) $(CFLAGS) -MM -MG $< -MF $@


#gcc manual says computed goto's may perform better with -fno-gcse
src/run.o: src/run.c
	$(CC) -c $(CFLAGS) -fno-gcse $< -o $@
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

$(STATIC_LIB_OUT):
	ar rcs $(STATIC_LIB_OUT) $(OBJECTS)

$(DYN_LIB_OUT): 
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.1 -o $(DYN_LIB_OUT)  $(OBJECTS)


# FLEX/BISON TARGETS
$(CMPLRPATH)/lex.yy.c: $(CMPLRPATH)/compsl.l $(CMPLRPATH)/compsl.y
	rm -f $(CMPLRPATH)/lex.yy.c
	flex -o$@ $<

$(CMPLRPATH)/compsl.tab.c: $(CMPLRPATH)/compsl.y
	rm -f $(CMPLRPATH)/compsl.tab.c
	bison -d $< -o $@


#$(CMPLRPATH)/compsl.tab.h: 
#$(CMPLRPATH)/compsl.tab.c
