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

COMPSL_VERSION := 0.1.1

.SUFFIXES:
#.SUFFIXES: .c .o .h .gch .dep

ifndef CC
CC      		= gcc
endif

BISON   		= bison
FLEX    		= flex
INSTALL 		= install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA 	= ${INSTALL} -m 644
LDCONFIG		= ldconfig
RANLIB			= ranlib

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
#libexecdir for programs run by this package and plugins and such
#should put that sort of thing in $(libexecdir)/<packagename>
libexecdir = $(exec_prefix)/libexec
datarootdir = $(prefix)/share
datadir = $(datarootdir)
sysconfdir = $(prefix)/etc
includedir = $(prefix)/include
docdir = $(datarootdir)/doc/compsl
htmldir = $(docdir)
libdir = $(exec_prefix)/lib
mandir = $(datarootdir)/man
man1dir = $(mandir)/man1
man2dir = $(mandir)/man2
man3dir = $(mandir)/man3
man4dir = $(mandir)/man4
man5dir = $(mandir)/man5
man6dir = $(mandir)/man6
man7dir = $(mandir)/man7
man8dir = $(mandir)/man8
manext = .1


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

CFLAGS  = -ftabstop=4 -Wall -Wbad-function-cast -Wcast-align -Wwrite-strings
CFLAGS += -Wnonnull
#CFLAGS += -Wunreachable-code

CFLAGS += -fsingle-precision-constant -ffast-math -fno-math-errno 
CFLAGS += -ffinite-math-only -fno-trapping-math 

CFLAGS += -funit-at-a-time -funroll-loops -funswitch-loops

ifeq ($(findstring MINGW,$(_ARCH)),MINGW)
	override WINDOWS := 1
else
	CPUTYPE := auto
endif

ifeq ($(CPUTYPE),auto)
		override CPUFLAGS := $(shell ./gcc-arch)
		#ifeq ($(findstring sse,$(CPUFLAGS)),sse)
		#	override CPUFLAGS += -mfpmath=sse,387
		#endif
		CFLAGS += $(CPUFLAGS)
else
	ifdef CPUTYPE
		CFLAGS += -march=$(CPUTYPE) 
	endif
	ifeq ($(MMX),1)
		CFLAGS += -mmmx  -DCOMPSL_MMX
	endif
	ifdef SSE
		CFLAGS += -DCOMPSL_SSE
		ifeq ($(SSE),1)
			CFLAGS += -msse -mfpmath=sse,387 
		else
			CFLAGS += -msse${SSE} -mfpmath=sse,387
		endif
	endif
endif

CFLAGS += -mno-ieee-fp

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
	CFLAGS += -ggdb3 -DDEBUG -fmudflap
	#CFLAGS += -D DEBUG $(DBG_ENVS)
	OPTIMIZE=0
else
	#for asserts
overide CFLAGS += -DNDEBUG
	OPTIMIZE=1
endif

ifeq ($(OPTIMIZE),1)
	CFLAGS += -O3 -fdata-sections -ffunction-sections -frename-registers
	CFLAGS += -fsingle-precision-constant -funroll-loops -finline-functions
	CFLAGS += -fsched-spec-load -maccumulate-outgoing-args
	CFLAGS += -minline-all-stringops -fomit-frame-pointer
	CFLAGS += -finline-limit=2000
	CFLAGS += -frename-registers
	CFLAGS += -fno-stack-limit
	#CFLAGS += -fbranch-target-load-optimize -fbranch-target-load-optimize2
	# -fsched2-use-superblocks
	#-fmove-all-movables

	# TODO: figure out if we need the -fno-strict-aliasing option.
	# TODO: make sure none of these breaks the library for linking....
else
	CFLAGS += -O0
endif

ifdef PIC
	COMPSL_PIC := -fpic
endif

# for shared library
ifndef WINDOWS
	override CFLAGS += -fvisibility=hidden
endif
override CFLAGS := $(shell CC=$(CC) ./gcc-optioncheck $(CFLAGS))

MYCFLAGS := -std=gnu99 -fbuiltin -D_GNU_SOURCE -DBUILDING_COMPSL
ALL_CFLAGS := ${CFLAGS} ${MYCFLAGS} ${COMPSL_PIC}


################################
# FILES                        #
################################

CMPATH=src/compiler
SHORTLIB=compsl
LIBNAME := lib$(SHORTLIB)

REG_SRCS:=src/compartment.c src/error.c  src/gen.c  src/run.c  src/vars.c src/vm.c \
	$(CMPATH)/binops.c $(CMPATH)/function.c $(CMPATH)/interncomp.c $(CMPATH)/err.c \
	$(CMPATH)/var.c $(CMPATH)/comp.c $(CMPATH)/control.c $(CMPATH)/compglobals.c \
	src/mt.c src/userspace.c $(CMPATH)/stackcheck.c $(CMPATH)/optimize.c

DERIVED_SRCS=$(CMPATH)/lex.yy.c $(CMPATH)/compsl.tab.c
DERIVED_FILES=$(DERIVED_SRCS) $(CMPATH)/compsl.tab.h 
SOURCES := $(REG_SRCS) $(DERIVED_SRCS)
OBJECTS := $(SOURCES:.c=.o)

TESTSRCS := $(addprefix src/test/,test-interp.c test-intern.c test-api.c test-comp.c test-torture.c)
TESTOBJS := $(TESTSRCS:.c=.o)

OTHERSRC := src/dumper.c
OTHEROBJ := $(OTHERSRC:.c=.o)

DEPS := $(SOURCES:.c=.dep) $(OTHERSRC:.c=.dep)

TEST_EXES := $(addprefix bin/,$(notdir $(basename $(TESTSRCS))))

STATIC_LIB_OUT := bin/$(LIBNAME).a
ifdef WINDOWS
	DYN_LIB_OUT := bin/$(SHORTLIB).dll
	DEFFILE     := bin/$(SHORTLIB).def
	IMPLIB      := bin/$(SHORTLIB).a
	LIBFILE     := bin/$(SHORTLIB).lib
	ALL_CFLAGS += -DWINDOWS
else
	DYN_LIB_OUT := bin/$(LIBNAME).so.$(COMPSL_VERSION)
endif

.PHONY: test cleantest all clean


################################
#TARGETS                       #
################################

all: $(STATIC_LIB_OUT) $(DYN_LIB_OUT)

install: all
	$(INSTALL) -d $(libdir)
	$(INSTALL) -d $(includedir)
	$(INSTALL) -m 755 $(STATIC_LIB_OUT) $(libdir)/$(notdir $(STATIC_LIB_OUT))
	$(INSTALL) -m 755 $(DYN_LIB_OUT) $(libdir)/$(notdir $(DYN_LIB_OUT))
	$(INSTALL) src/extern/compsl.h $(includedir)/compsl.h
	$(LDCONFIG) -n $(libdir)
	$(RANLIB) $(libdir)/$(notdir $(STATIC_LIB_OUT))
	
install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' install

strip: all
	strip --strip-unneeded $(STATIC_LIB_OUT) $(DYN_LIB_OUT)

clean:
	-rm -f -- $(OBJECTS) $(TESTOBJS) $(OTHEROBJ) $(STATIC_LIB_OUT) \
		 $(DYN_LIB_OUT) $(CMPRL_TEST_EXE) $(DEPS) $(TEST_EXES:=*)  \
		 $(DERIVED_FILES) $(CMPATH)/compsl.output bin/dumper*      \
		 $(EXPORTS) $(DEFFILE) $(LIBFILE) $(IMPLIB)

help: 
	@echo "Makefile for CompSL"
	@echo "  Targets: all, clean, test, test-valgrind, static, dynamic, package"
	@echo "  Variables: DEBUG, TRACE_INTERP, DEBUG_COMP, STACK_CHECK(=1 by default)"

static: $(STATIC_LIB_OUT)
dynamic: $(DYN_LIB_OUT)

package: clean
	rm -f compsl-${COMPSL_VERSION}.tar.bz2
	tar --exclude "*/.svn*" --exclude "*/.settings*" \
		--exclude "*/.cvsignore" --exclude "*/.*project*" \
		--exclude "*~" \
		--exclude "*/compsl-${COMPSL_VERSION}/compsl-${COMPSL_VERSION}"\
		--transform 's,^,compsl-${COMPSL_VERSION}/,'\
		-cjvf compsl-${COMPSL_VERSION}.tar.bz2 *

bin/dumper: src/dumper.o $(OBJECTS)
	$(CC) -MD $< $(OBJECTS) $(PLATLIBS) ${MYCFLAGS} -o $@

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

ifdef WINDOWS
$(DYN_LIB_OUT) $(DEFFILE) $(IMPLIB): $(OBJECTS)
	$(CC) $(ALL_CFLAGS) $(OBJECTS) -shared \
		-Wl,--out-implib,$(IMPLIB)  \
		-Wl,--output-def,$(DEFFILE) \
		-Wl,-soname,$(DYN_LIB_OUT)  \
		-Wl,--add-stdcall-alias     \
		-o $(DYN_LIB_OUT)
else
$(DYN_LIB_OUT): $(OBJECTS)
	$(CC) -shared -Wl,-soname,lib$(SHORTLIB).so $(OBJECTS) $(PLATLIBS) -o $(DYN_LIB_OUT) $(ALL_CFLAGS)
endif

$(STATIC_LIB_OUT): $(OBJECTS)
	ar rcs $(STATIC_LIB_OUT) $(OBJECTS)


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
	$(CC) -MD $< $(OBJECTS) $(PLATLIBS) ${MYCFLAGS} -o $@
