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

COMPSL_VERSION := 0.1.2

.SUFFIXES:
#.SUFFIXES: .c .o .h .gch .dep

ifeq ($(origin CC),default)
CC      		= gcc
endif

BISON   		= bison
FLEX    		= flex
LIBTOOL         = libtool
INSTALL 		= install
INSTALL_PROGRAM = $(INSTALL) -v
INSTALL_DATA 	= ${INSTALL} -v -m 644
LDCONFIG		= ldconfig
RANLIB			= ranlib
DOXYGEN			= doxygen

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

override CPUGUESS=0
ifeq ($(CPUTYPE),auto)
	override CPUFLAGS = $(shell ./gcc-arch &2>/dev/null)
	override CPUGUESS=1
else
	override CPUFLAGS =
	ifdef CPUTYPE
		override CPUFLAGS += -march=$(CPUTYPE) 
	endif
	ifeq ($(MMX),1)
		override CPUFLAGS += -mmmx
	endif
	ifdef SSE
		ifeq ($(SSE),1)
			override CPUFLAGS += -msse -mfpmath=sse,387 
		else
			override CPUFLAGS += -msse${SSE} -mfpmath=sse,387
		endif
	endif
endif
CFLAGS += $(CPUFLAGS)

CFLAGS += -mno-ieee-fp

override APPSTATMSG=

ifdef DEBUG_COMP
	CFLAGS += -DDEBUG_COMP
	override APPSTATMSG += Compiler Debug messages are ON\\n
else
	override APPSTATMSG += Compiler Debug messages are OFF\\n
endif

ifdef TRACE_INTERP
	CFLAGS += -D_COMPSL_TRACE
	override APPSTATMSG += Interpreter execution tracing is ON\\n
else
	override APPSTATMSG += Interpreter execution tracing is OFF\\n
endif

#cause the optimizer to output code dumps as it runs each stage
ifdef TRACE_OPTIM
	CFLAGS += -DCOMPSL_TRACE_OPTIM
	override APPSTATMSG += Tracing bytecode optimizer is ON\\n
else
	override APPSTATMSG += Tracing bytecode optimizer is OFF\\n
endif

# make the compiler output the line number it's currently compiling 
# to stdout
ifdef TRACE_COMPILE
	CFLAGS += -DCOMPSL_TRACE_COMPILE
	override APPSTATMSG += Tracing compile is ON\\n
else
	override APPSTATMSG += Tracing compile is OFF\\n
endif

#allow overrides from command line, but enable by default
STACK_CHECK = 1
ifeq ($(STACK_CHECK), 1)
	CFLAGS += -DCOMP_STACKCHECK
	override APPSTATMSG += Compile time bytecode stack bounds checking is ON\\n
else
	override APPSTATMSG += Compile time bytecode stack bounds checking is OFF\\n
endif

# note: not needed on x86 apparntly
ifdef PIC
	COMPSL_PIC := -fpic
	override APPSTATMSG += Position independant code generation ON\\n
endif

ifdef DEBUG
	# mudflap is some pointer debuging stuff
	# disable builtins since we can't set breakpoints on
	# calls to builtin functions
	# stack protector checks for stack overruns
	CFLAGS += -ggdb3 -DDEBUG -fmudflap -fno-builtin -fstack-protector
	#CFLAGS += -D DEBUG $(DBG_ENVS)
	OPTIMIZE=0
else
	#for asserts
overide CFLAGS += -DNDEBUG
	OPTIMIZE=FULL
endif

ifeq ($(OPTIMIZE),FULL)
	CFLAGS += -O3 -fdata-sections -ffunction-sections -frename-registers
	CFLAGS += -fsingle-precision-constant -funroll-loops -finline-functions
	CFLAGS += -fsched-spec-load -maccumulate-outgoing-args
	CFLAGS += -minline-all-stringops -fomit-frame-pointer
	CFLAGS += -finline-limit=2000
	CFLAGS += -frename-registers
	CFLAGS += -fno-stack-limit
	CFLAGS += -funswitch-loops
	

	# TODO: figure out if we need the -fno-strict-aliasing option.
	# TODO: make sure none of these breaks the library for linking....
	CFLAGS += -fstrict-aliasing -Wstrict-aliasing=2 
	
	CFLAGS += -fgcse-sm -fgcse-las -fgcse-after-reload
	CFLAGS += -ftree-vectorize
	CFLAGS += -ftracer
	CFLAGS += -fvariable-expansion-in-unroller -fprefetch-loop-arrays
	CFLAGS += -freorder-blocks-and-partition
	
	CFLAGS += -fbranch-target-load-optimize 
	#CFLAGS += -fbranch-target-load-optimize2
	CFLAGS += -floop-optimize2 -fmove-all-movables
	
	###################################################
	# potentially bad optimizations
	###################################################
	# from gcc manual
	# This option is experimental, as not all machine descriptions used by GCC 
	# model the CPU closely enough to avoid unreliable results from the algorithm.
	CFLAGS += -fsched2-use-superblocks
else
	CFLAGS += -O$(OPTIMIZE)
endif

# for shared library
ifndef WINDOWS
	override CFLAGS += -fvisibility=hidden
endif

override CFLAGS := $(shell CC=$(CC) ./gcc-optioncheck $(CFLAGS))

MYCFLAGS := -std=gnu99 -fbuiltin -D_GNU_SOURCE -DBUILDING_COMPSL -Wno-attributes
ALL_CFLAGS := ${CFLAGS} ${MYCFLAGS} ${COMPSL_PIC}


STATMSG = Compiling with $(CC) on $(_ARCH) for
ifdef DEBUG
	STATMSG += Debugging\\n
else
	STATMSG += Release\\n
endif
ifeq ($(OPTIMIZE),0)
	STATMSG += Optimization is OFF\\n
else
	STATMSG += Optimization is ON at $(OPTIMIZE)\\n
endif
ifdef CPUTYPE
	ifeq ($(CPUGUESS),1)
		STATMSG += Guessed CPUTYPE of $(patsubst -march=%,%,$(filter -march% ,$(CPUFLAGS)))\\n
		STATMSG +=    Adding CPU flags $(CPUFLAGS)\\n
	else
		STATMSG += Compiling for $(CPUTYPE) with $(CPUFLAGS)\\n
	endif
endif
ifdef PLATLIBS
	STATMSG += Platform specifice librarys to link $(patsubst -l%,lib%,$(PLATLIBS))\\n
endif
STATMSG += $(APPSTATMSG)

STATMSG += \\nTools\\n

STATMSG +=BISON           = $(BISON)\\n
STATMSG +=FLEX            = $(FLEX)\\n
STATMSG +=LIBTOOL         = $(LIBTOOL)\\n
STATMSG +=INSTALL         = $(INSTALL)\\n
STATMSG +=INSTALL_PROGRAM = $(INSTALL_PROGRAM)\\n
STATMSG +=INSTALL_DATA    = $(INSTALL_DATA)\\n
STATMSG +=LDCONFIG        = $(LDCONFIG)\\n
STATMSG +=RANLIB          = $(RANLIB)\\n
STATMSG +=DOXYGEN         = $(DOXYGEN)\\n

STATMSG +=\\nDirectories\\n
STATMSG +=prefix      = $(prefix)\\n
STATMSG +=exec_prefix = $(prefix)\\n
STATMSG +=bindir      = $(bindir)\\n
STATMSG +=libexecdir  = $(libexecdir)\\n
STATMSG +=datarootdir = $(datarootdir)\\n
STATMSG +=datadir     = $(datadir)\\n
STATMSG +=sysconfdir  = $(sysconfdir)\\n
STATMSG +=includedir  = $(includedir)\\n
STATMSG +=docdir      = $(docdir)\\n
STATMSG +=htmldir     = $(htmldir)\\n
STATMSG +=libdir      = $(libdir)\\n
STATMSG +=mandir      = $(mandir)\\n

STATMSG +=\\n

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

OTHERSRC := src/dumper.c src/perf-test.c
OTHEROBJ := $(OTHERSRC:.c=.o)

DEPS := $(SOURCES:.c=.dep) $(OTHERSRC:.c=.dep) $(TESTSRCS:.c=.dep)

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

DOXYFILE = compsl.doxyfile

.PHONY: test cleantest all clean docs install install-strip help package
.PHONY: test-valgrind statmsg testmsg
.INTERMEDIATE: $(OBJECTS) $(TESTOBJS) $(OTHEROBJ) 
.SECONDARY: $(DERIVED_FILES) $(CMPATH)/compsl.output
################################
#TARGETS                       #
################################

all: statmsg $(STATIC_LIB_OUT) $(DYN_LIB_OUT) 

install: all docs
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
		 $(DERIVED_FILES) $(CMPATH)/compsl.output                  \
		 $(EXPORTS) $(DEFFILE) $(LIBFILE) $(IMPLIB)                \
		 bin/perf-test* bin/dumper* 
	-rm -rf	doc/html doc/latex doc/man
	@echo

docs: $(DOXYFILE)
	$(DOXYGEN) $(DOXYFILE)
	make -C doc/latex
	mv doc/latex/refman.pdf doc/compsl.pdf

help: 
	@echo "Makefile for CompSL"
	@echo "  Targets: all, clean, test, test-valgrind, static, dynamic, package, docs"
	@echo "  Variables: DEBUG, TRACE_INTERP, DEBUG_COMP, STACK_CHECK(=1 by default)"
	@echo
	@echo -ne $(STATMSG)
	@echo -e "CFLAGS\n$(ALL_CFLAGS)" | fold -s

static: statmsg $(STATIC_LIB_OUT) 
dynamic: statmsg $(DYN_LIB_OUT) 

package: clean
	rm -f compsl-${COMPSL_VERSION}.tar.bz2
	tar --exclude "*/.svn*" --exclude "*/.settings*" \
		--exclude "*/.cvsignore" --exclude "*/.*project*" \
		--exclude "*~" --exclude "*/latex*"\
		--exclude "*/compsl-${COMPSL_VERSION}/compsl-${COMPSL_VERSION}"\
		--transform 's,^,compsl-${COMPSL_VERSION}/,'\
		-cjvf compsl-${COMPSL_VERSION}.tar.bz2 *

bin/dumper: src/dumper.o $(OBJECTS)
	$(CC) $(ALL_CFLAGS) -MD $< $(OBJECTS) $(PLATLIBS) -o $@

bin/perf-test: src/perf-test.o $(OBJECTS)
	$(CC) $(ALL_CFLAGS) -MD $< $(OBJECTS) $(PLATLIBS) -o $@

cleantest: clean test 

test: statmsg $(TEST_EXES) testmsg $(addprefix run-,$(notdir $(basename $(TESTSRCS)))) 


test-valgrind: $(TEST_EXES)
	@for test in $^; do \
		echo Running $$test; \
		valgrind $$test ; \
		echo DONE; \
	done

################################
# Message targets              #
################################
testmsg:
	@echo -ne "\n\nRUNNING TESTS\n"
	@echo -ne "***************************************************\n"
statmsg:
	@echo -ne "\nSETTINGS\n"
	@echo -ne "***************************************************\n"
	@echo -ne $(STATMSG)
	@echo -e "CFLAGS\n$(ALL_CFLAGS)\n" | fold -s
	@sleep 5
	@echo -ne "\nSTARTING OPERATION\n"
	@echo -ne "***************************************************\n"
################################
# INTERNAL TARGETS             #
################################

#Dash makes it not error if not found
-include $(DEPS)

#gcc manual says computed goto's may perform better with -fno-gcse
src/run.o: src/run.c
	@$(CC) -MM $(ALL_CFLAGS) -fno-gcse -Wno-unused-label $< > src/run.dep
	@$(CC) -c  $(ALL_CFLAGS) -fno-gcse -Wno-unused-label $< -o $@

%.o: %.c
	@$(CC) -MM $(ALL_CFLAGS) $*.c > $*.dep
	@$(CC) -c  $(ALL_CFLAGS) $< -o $@

ifdef WINDOWS
$(DYN_LIB_OUT) $(DEFFILE) $(IMPLIB): $(OBJECTS)
	@$(CC) $(ALL_CFLAGS) $(OBJECTS) -shared \
		-Wl,--out-implib,$(IMPLIB)  \
		-Wl,--output-def,$(DEFFILE) \
		-Wl,-soname,$(DYN_LIB_OUT)  \
		-Wl,--add-stdcall-alias     \
		-o $(DYN_LIB_OUT)
else
$(DYN_LIB_OUT): $(OBJECTS)
	@$(CC) -shared -Wl,-soname,lib$(SHORTLIB).so $(OBJECTS) $(PLATLIBS) -o $(DYN_LIB_OUT) $(ALL_CFLAGS)
endif

$(STATIC_LIB_OUT): $(OBJECTS)
	@ar rcs $(STATIC_LIB_OUT) $(OBJECTS)


################################
# FLEX/BISON TARGETS           #
################################

$(CMPATH)/binops.c: $(CMPATH)/compsl.tab.h
$(CMPATH)/compsl.tab.h: $(CMPATH)/compsl.tab.c

$(CMPATH)/compsl.tab.c: $(CMPATH)/compsl.y
	@rm -f $(CMPATH)/compsl.tab.c $(CMPATH)/compsl.tab.h
	@$(BISON) --report all -d  $(CMPATH)/compsl.y -o $(CMPATH)/compsl.tab.c

$(CMPATH)/lex.yy.c: $(CMPATH)/compsl.l $(CMPATH)/compsl.tab.h
	@rm -f $(CMPATH)/lex.yy.c
	@$(FLEX) -o$@ $<


####################################################
# Testers - assume each test is one sourcefile
####################################################
run-test-%: bin/test-%
	@$<
	@echo

bin/test-%: src/test/test-%.o $(STATIC_LIB_OUT)
	@$(CC) $(ALL_CFLAGS) -MD $< $(OBJECTS) $(PLATLIBS) -o $@
