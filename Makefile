

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


BISON = bison
FLEX  = flex
CC    = gcc


################################
# FLAGS                        #
################################
ifndef _ARCH
	_ARCH := $(strip $(shell uname -s))
	export _ARCH
endif

ifeq ($(_ARCH),Linux)
	PLATLIBS := -lm 
endif


CFLAGS  := -ftabstop=4 -Wall -Wbad-function-cast -Wcast-align -Wwrite-strings
#CFLAGS += -Wunreachable-code

CFLAGS += -fsingle-precision-constant -ffast-math

CPUTYPE=auto

ifeq ($(CPUTYPE),auto)
		override CPUFLAGS := $(shell ./gcc-arch) -mno-ieee-fp
		ifeq ($(findstring sse,$(CPUFLAGS)),sse)
			override CPUFLAGS += -mfpmath=sse
		endif
		CFLAGS += $(CPUFLAGS)
else
	ifdef CPUTYPE
		CFLAGS += -mcpu=$(CPUTYPE) 
	endif
	ifeq ($(MMX),1)
		CFLAGS += -mmmx -mno-ieee-fp
	endif
	ifdef SSE
		CFLAGS += -msse${SSE} -mfpmath=sse -mno-ieee-fp 
	endif
endif


ifdef DEBUG_COMP
	CFLAGS += -DDEBUG_COMP
endif

ifdef TRACE_INTERP
	CFLAGS += -D_COMPSL_TRACE
endif

#allow overrides from command line, but enable by default
STACK_CHECK = 1
ifeq ($(STACK_CHECK), 1)
	CFLAGS += -DCOMP_STACKCHECK
endif


ifdef DEBUG
	CFLAGS += -O0 -ggdb3 -DDEBUG
	#CFLAGS += -D DEBUG $(DBG_ENVS)
else
	CFLAGS += -O2 -fdata-sections -ffunction-sections -frename-registers
	CFLAGS += -fsingle-precision-constant -funroll-loops -finline-functions
	CFLAGS += -fsched-spec-load -maccumulate-outgoing-args
	CFLAGS += -minline-all-stringops
	#CFLAGS += -fbranch-target-load-optimize -fsched2-use-superblocks
	#-fmove-all-movables

	#for asserts
overide CFLAGS += -DNDEBUG

	# TODO: figure out if we need the -fno-strict-aliasing option.
	# TODO: make sure none of these breaks the library for linking....
endif

MYCFLAGS := -std=gnu99 -fbuiltin -D_GNU_SOURCE
ALL_CFLAGS := ${CFLAGS} ${MYCFLAGS}


################################
# FILES                        #
################################

CMPATH=src/compiler
SHORTLIB=compsl
LIBNAME := lib$(SHORTLIB)

REG_SRCS=src/compartment.c src/error.c  src/gen.c  src/run.c  src/vars.c src/vm.c \
	$(CMPATH)/binops.c $(CMPATH)/function.c $(CMPATH)/interncomp.c $(CMPATH)/err.c \
	$(CMPATH)/var.c $(CMPATH)/comp.c $(CMPATH)/control.c $(CMPATH)/compglobals.c \
	src/mt.c src/userspace.c $(CMPATH)/stackcheck.c

DERIVED_SRCS=$(CMPATH)/lex.yy.c $(CMPATH)/compsl.tab.c
DERIVED_FILES=$(DERIVED_SRCS) $(CMPATH)/compsl.tab.h 
SOURCES := $(REG_SRCS) $(DERIVED_SRCS)
OBJECTS := $(SOURCES:.c=.o)

TESTSRCS := $(addprefix src/test/,test-interp.c test-intern.c test-api.c test-comp.c test-torture.c)
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

help: 
	@echo "Makefile for CompSL"
	@echo "  Targets: all, clean, test, test-valgrind, static, dynamic"
	@echo "  Variables: DEBUG, TRACE_INTERP, DEBUG_COMP"

static: $(STATIC_LIB_OUT)
dynamic: $(DYN_LIB_OUT)

cleantest: clean test 

test: $(TEST_EXES)
	@for test in $^; do \
		echo Running $$test; \
		$$test ; \
		echo DONE; echo; \
	done

test-valgrind: $(TEST_EXES)
	@for test in $^; do \
		echo Running $$test; \
		valgrind $$test ; \
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
	$(CC) -MM $(ALL_CFLAGS) $*.c > $*.dep
	$(CC) -c $(ALL_CFLAGS) $< -o $@


$(STATIC_LIB_OUT): $(OBJECTS)
	ar rcs $(STATIC_LIB_OUT) $(OBJECTS)

$(DYN_LIB_OUT): $(OBJECTS)
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.1 $(OBJECTS) $(PLATLIBS) -o $(DYN_LIB_OUT)

################################
# FLEX/BISON TARGETS           #
################################

$(CMPATH)/binops.c: $(CMPATH)/compsl.tab.h
$(CMPATH)/compsl.tab.h: $(CMPATH)/compsl.tab.c

$(CMPATH)/compsl.tab.c: $(CMPATH)/compsl.y
	rm -f $(CMPATH)/compsl.tab.c $(CMPATH)/compsl.tab.h
	$(BISON) --report all -d  $(CMPATH)/compsl.y -o $(CMPATH)/compsl.tab.c

$(CMPATH)/lex.yy.c: $(CMPATH)/compsl.l $(CMPATH)/compsl.tab.h
	rm -f $(CMPATH)/lex.yy.c
	$(FLEX) -o$@ $<


####################################################
# Testers - assume each test is one sourcefile
####################################################
bin/test-%: src/test/test-%.o $(STATIC_LIB_OUT)
	$(CC) ${MYCFLAGS} -MD $< $(OBJECTS) $(PLATLIBS) -o $@
