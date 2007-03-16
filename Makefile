

#################################
# Build compsl                  #
#################################
#Created: 17Mar06
#
#Links
# http://www.adp-gmbh.ch/cpp/gcc/create_lib.html
# http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# http://www.gnu.org/software/make/manual/make.html

#TODO: figure out how to make the test link against version of the lib with debug
# symbols but also have optimizations in the release worthy stuff ie: end up with two
# versions of the static lib, one with debug stuff and not optimization, and the other
# with no debug info and -O2

#Figure out how we wanna make libm get linked with tests on linux

DEBUG = 1

ifndef _ARCH
	_ARCH := $(strip $(shell uname -s))
	export _ARCH
endif

ifeq ($(_ARCH),Linux)
	PLATLIBS := -lm 
endif
#TODO: fix here
DBG_MODS=DEBUG_COMP DEBUG_FOO
DBG_ENVS=$(foreach cur,$(DBG_MODS), $(if $(ifeq $($(cur)) ''), -D $(cur)) )

BISON = bison
FLEX = flex

CFLAGS  = -fsingle-precision-constant
CFLAGS += -ftabstop=4 -Wall -Wbad-function-cast -Wcast-align -Wwrite-strings -Wunreachable-code


#NOTE: on linux we seem to need to link against libm, so we need to adjust some of the linking
#to do this... DONE!

# if none x86 need to disable this line
#ALL_CFLAGS += -mmmx -mno-ieee-fp
#if sse instructions not available need to disable this line
CFLAGS += -msse -mfpmath=sse

ifdef DEBUG
	CFLAGS += -O0 -ffast-math -ggdb 
	#CFLAGS += -D DEBUG $(DBG_ENVS)
else
	CFLAGS += -O2 -ffast-math -fdata-sections -ffunction-sections -fbranch-target-load-optimize -frename-registers -fsingle-precision-constant -funroll-loops -finline-functions -fsched-spec-load -fsched2-use-superblocks 

#-fmove-all-movables

	# TODO: figure out if we need the -fno-strict-aliasing option.
	# TODO: make sure none of these breaks the library for linking....
endif

MYCFLAGS := -std=gnu99 -fbuiltin
ALL_CFLAGS = ${CFLAGS} ${MYCFLAGS}

CC=gcc

# Fixes for building on UofT's computers
#CC=gcc-3.4

CMPLRPATH=src/compiler
SHORTLIB=compsl
LIBNAME := lib$(SHORTLIB)

REG_SRCS=src/compartment.c src/error.c  src/gen.c  src/run.c  src/vars.c src/vm.c src/compiler/comp.c src/mt.c

DERIVED_SRCS=$(CMPLRPATH)/lex.yy.c $(CMPLRPATH)/compsl.tab.c
DERIVED_FILES=$(DERIVED_SRCS) src/compiler/compsl.tab.h 
SOURCES := $(REG_SRCS) $(DERIVED_SRCS)
OBJECTS := $(SOURCES:.c=.o)

TESTSRCS := $(addprefix src/test/,test-interp.c test-comp.c test-api.c)
TESTOBJS := $(TESTSRCS:.c=.o)

DEPS := $(SOURCES:.c=.dep) 

TEST_EXES := $(addprefix bin/,$(notdir $(basename $(TESTSRCS))))

STATIC_LIB_OUT := bin/$(LIBNAME).a
DYN_LIB_OUT := bin/$(LIBNAME).so.1.0.1

.PHONY: test cleantest all clean


################################
#TARGETS                       #
################################

all: $(STATIC_LIB_OUT) $(DYN_LIB_OUT)

clean:
	-rm -f -- $(OBJECTS) $(TESTOBJS) $(STATIC_LIB_OUT) $(DYN_LIB_OUT) $(CMPRL_TEST_EXE) \
		$(DEPS) $(TEST_EXES:=*) $(DERIVED_FILES)

static: $(STATIC_LIB_OUT)

dynamic: $(DYN_LIB_OUT)

cleantest: clean test 

test: $(TEST_EXES)
	@for test in $^; do \
		echo Running $$test; \
		$$test ; \
		echo DONE; \
	done

################################
# INTERNAL TARGETS             #
################################

#Dash makes it not error if not found
-include $(DEPS)

#gcc manual says computed goto's may perform better with -fno-gcse
src/run.o: src/run.c
	$(CC) -c $(ALL_CFLAGS) -fno-gcse $< $(PLATLIBS) -o $@
	$(CC) -MM $(ALL_CFLAGS) src/run.c > src/run.dep

%.o: %.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@
	$(CC) -MM $(ALL_CFLAGS) $*.c > $*.dep

$(STATIC_LIB_OUT): $(OBJECTS)
	ar rcs $(STATIC_LIB_OUT) $(OBJECTS)

$(DYN_LIB_OUT): $(OBJECTS)
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.1 $(OBJECTS) $(PLATLIBS) -o $(DYN_LIB_OUT)

################################
# FLEX/BISON TARGETS           #
################################

$(CMPLRPATH)/compsl.tab.c $(CMPLRPATH)/compsl.tab.h: $(CMPLRPATH)/compsl.y
	rm -f $(CMPLRPATH)/compsl.tab.c $(CMPLRPATH)/compsl.tab.h
	$(BISON) -d $< -o $@

$(CMPLRPATH)/lex.yy.c: $(CMPLRPATH)/compsl.l $(CMPLRPATH)/compsl.tab.h
	rm -f $(CMPLRPATH)/lex.yy.c
	$(FLEX) -o$@ $<


####################################################
# Testers - aussume each test is one sourcefile
####################################################
bin/test-%: src/test/test-%.o $(STATIC_LIB_OUT)
	$(CC) ${MYCFLAGS} -MD -static $< $(OBJECTS) $(PLATLIBS) -o $@
