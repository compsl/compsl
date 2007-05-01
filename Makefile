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

ifndef _ARCH
	_ARCH := $(strip $(shell uname -s))
	export _ARCH
endif

-include config.mak


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
.PHONY: test-valgrind statmsg testmsg test-exes
#.INTERMEDIATE: 
.SECONDARY: $(OBJECTS) $(DERIVED_FILES) $(CMPATH)/compsl.output
.SECONDARY: $(TESTOBJS) $(OTHEROBJ)

################################
#TARGETS                       #
################################

all: statmsg $(STATIC_LIB_OUT) $(DYN_LIB_OUT) ;

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
	@-rm -f -- $(OBJECTS) $(TESTOBJS) $(OTHEROBJ) $(DEPS) 
	-rm -f -- $(DYN_LIB_OUT) $(STATIC_LIB_OUT) $(TEST_EXES:=*)
	-rm -f -- $(DERIVED_FILES) $(CMPATH)/compsl.output
	-rm -f -- bin/perf-test* bin/dumper* 
	-rm -f -- $(DEFFILE) $(LIBFILE) $(IMPLIB)
	-rm -rf   doc/html doc/latex doc/man
	@echo

docs: $(DOXYFILE)
	$(DOXYGEN) $(DOXYFILE)
	make -C doc/latex
	mv doc/latex/refman.pdf doc/compsl.pdf

help: 
	@echo -e "\nMakefile for CompSL"
	@echo -ne "***************************************************\n"
	@echo "  Targets: all, clean, test, test-valgrind, static, dynamic, package,"
	@echo "           docs, test-exes"
	@echo
	@echo "  Variables: DEBUG, TRACE_INTERP, DEBUG_COMP, STACK_CHECK(=1 by default)"
	@echo

static: statmsg $(STATIC_LIB_OUT) ;
dynamic: statmsg $(DYN_LIB_OUT) ;

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

cleantest: clean test ;

test: test-exes testmsg $(addprefix run-,$(notdir $(basename $(TESTSRCS)))) ;

test-exes: statmsg $(TEST_EXES) ;

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
src/run.o: src/run.c config.mak
	@echo CC $<
	@$(CC) -MM -MQ $@ $(ALL_CFLAGS) -fno-gcse -Wno-unused-label $< > src/run.dep
	@$(CC) -c  $(ALL_CFLAGS) -fno-gcse -Wno-unused-label $< -o $@

%.o: %.c config.mak
	@echo CC $<
	@$(CC) -MM -MQ $@ $(ALL_CFLAGS) $*.c > $*.dep
	@$(CC) -c  $(ALL_CFLAGS) $< -o $@

ifdef WINDOWS
$(DYN_LIB_OUT) $(DEFFILE) $(IMPLIB): $(OBJECTS)
	@echo LINK $@
	@$(CC) $(ALL_CFLAGS) $(OBJECTS) -shared \
		-Wl,--out-implib,$(IMPLIB)  \
		-Wl,--output-def,$(DEFFILE) \
		-Wl,-soname,$(DYN_LIB_OUT)  \
		-Wl,--add-stdcall-alias     \
		-o $(DYN_LIB_OUT)
else
$(DYN_LIB_OUT): $(OBJECTS)
	@echo LINK $@
	@$(CC) -shared -Wl,-soname,lib$(SHORTLIB).so $(OBJECTS) $(PLATLIBS) -o $(DYN_LIB_OUT) $(ALL_CFLAGS)
endif

$(STATIC_LIB_OUT): $(OBJECTS)
	@echo AR $@
	@ar rcs $(STATIC_LIB_OUT) $(OBJECTS)


################################
# FLEX/BISON TARGETS           #
################################

$(CMPATH)/binops.c: $(CMPATH)/compsl.tab.h
$(CMPATH)/compsl.tab.h: $(CMPATH)/compsl.tab.c

$(CMPATH)/compsl.tab.c: $(CMPATH)/compsl.y
	@rm -f $(CMPATH)/compsl.tab.c $(CMPATH)/compsl.tab.h
	@echo BISON $<
	@$(BISON) --report all -d  $(CMPATH)/compsl.y -o $(CMPATH)/compsl.tab.c

$(CMPATH)/lex.yy.c: $(CMPATH)/compsl.l $(CMPATH)/compsl.tab.h
	@rm -f $(CMPATH)/lex.yy.c
	@echo FLEX $<
	@$(FLEX) -o$@ $<


####################################################
# Testers - assume each test is one sourcefile
####################################################
run-test-%: bin/test-%
	@echo
	@$<
	@echo

bin/test-%: src/test/test-%.o $(STATIC_LIB_OUT)
	@echo LINK $@
	@$(CC) $(ALL_CFLAGS) -MD $< $(OBJECTS) $(PLATLIBS) -o $@

####################################################
config.mak: configure
	@sh configure