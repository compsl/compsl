#################################
# Build compsl                  #
#################################
#Created: 17Mar06
#
#Links
# http://www.adp-gmbh.ch/cpp/gcc/create_lib.html
# http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# http://www.gnu.org/software/make/manual/make.html
# http://www.fortran-2000.com/ArnaudRecipes/sharedlib.html


#TODO: figure out how to make the test link against version of the lib with debug
# symbols but also have optimizations in the release worthy stuff ie: end up with two
# versions of the static lib, one with debug stuff and not optimization, and the other
# with no debug info and -O2

COMPSL_VERSION := 0.2.3

.SUFFIXES:

HAVE_CONFIG_OPTS=$(shell if [ -f config.opts ]; then echo yes; else echo no; fi)
ifeq ($(HAVE_CONFIG_OPTS),yes)
	CONFIG_OPTS = $(shell if [ -f config.opts ]; then cat config.opts; fi)
endif

include setup.mk

################################
# FILES                        #
################################

CMPATH=src/compiler
SHORTLIB=compsl
LIBNAME := lib$(SHORTLIB)

GEN_HEADERS:=src/include/intern/bcstrings.h \
	src/interp/jumptbl.h src/include/compiler/bc_info.h \
	src/include/intern/bytecode.h $(CMPATH)/compsl.tab.h

REG_SRCS:=src/api/compartment.c src/api/error.c  src/api/gen.c  src/interp/run.c \
	src/api/vars.c src/api/vm.c src/api/mt.c src/api/userspace.c \
	$(CMPATH)/binops.c $(CMPATH)/function.c $(CMPATH)/interncomp.c $(CMPATH)/err.c \
	$(CMPATH)/var.c $(CMPATH)/comp.c $(CMPATH)/control.c $(CMPATH)/compglobals.c \
	$(CMPATH)/stackcheck.c $(CMPATH)/optimize.c

DERIVED_SRCS=$(CMPATH)/lex.yy.c $(CMPATH)/compsl.tab.c
DERIVED_FILES=$(DERIVED_SRCS) $(CMPATH)/compsl.tab.h 
SOURCES := $(REG_SRCS) 
OBJECTS := $(SOURCES:.c=.o) $(DERIVED_SRCS:.c=.o)

TESTSRCS := $(addprefix src/test/test-,interp-base.c interp-jumps.c \
    interp-arith.c interp-comp.c interp-builtins.c interp.c intern.c api.c \
    comp.c torture.c so.c)

TESTOBJS := $(TESTSRCS:.c=.o)

OTHERSRC := src/dumper.c src/perf-test.c src/runner.c
OTHEROBJ := $(OTHERSRC:.c=.o)

DEPS := $(SOURCES:.c=.dep) $(OTHERSRC:.c=.dep) $(TESTSRCS:.c=.dep)
DDEPS := $(DERIVED_SRCS:.c=.ddp)

TEST_EXES := $(addprefix bin/,$(notdir $(basename $(TESTSRCS))))

STATIC_LIB_OUT := bin/$(LIBNAME).a
ifeq ($(TARGET_WIN32),yes)
	DYN_LIB_OUT := bin/$(SHORTLIB).dll
	DEFFILE     := bin/$(SHORTLIB).def
	IMPLIB      := bin/$(SHORTLIB).a
	LIBFILE     := bin/$(SHORTLIB).lib
else
	DYN_LIB_OUT := bin/$(LIBNAME).so.$(COMPSL_VERSION)
endif

DOXYFILE = compsl.doxyfile

.PHONY: test cleantest all clean docs install install-strip help package
.PHONY: test-valgrind statmsg testmsg test-exes distclean autoconfig
.INTERMEDIATE: compsl.tab.h
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

distclean: clean
	-rm -f config.mak src/intern/config.h
clean:
	@-rm -f -- $(OBJECTS) $(TESTOBJS) $(OTHEROBJ) $(DEPS) $(DDEPS)
	-rm -f -- $(DYN_LIB_OUT) $(STATIC_LIB_OUT) $(TEST_EXES:=*)
	-rm -f -- $(CMPATH)/compsl.output $(CMPATH)/compsl.tab.h 
	-rm -f -- bin/perf-test* bin/dumper* bin/runner*
	-rm -f -- $(DEFFILE) $(LIBFILE) $(IMPLIB)
	-rm -rf   doc/html doc/latex doc/man
	-rm -f compsl.tab.h
	@echo

docs: $(DOXYFILE)
	$(DOXYGEN) $(DOXYFILE)
	make -C doc/latex
	mv doc/latex/refman.pdf doc/compsl.pdf

headers: $(GEN_HEADERS)

clean-headers:
	rm -f $(GEN_HEADERS)

help: 
	@printf "\nMakefile for CompSL\n"
	@printf "***************************************************\n"
	@printf "  Targets: all, clean, test, test-valgrind, static, dynamic, package,\n"
	@printf "           docs, test-exes\n"
	@printf
	@printf "  Variables: DEBUG, TRACE_INTERP, DEBUG_COMP, STACK_CHECK(=1 by default)\n"
	@printf "\n"
	@printf "  Please run ./configure to set up compilation, for help with configure\n"
	@printf "           run ./configure --help\n"
	@echo

static: statmsg $(STATIC_LIB_OUT) ;
dynamic: statmsg $(DYN_LIB_OUT) ;

package: distclean headers
	rm -f compsl-${COMPSL_VERSION}.tar.bz2
	tar --exclude "*/.svn*" --exclude "*/.settings*" \
		--exclude "*/.cvsignore" --exclude "*/.*project*" \
		--exclude "*~" --exclude "*/latex*" --exclude "*/config.*" \
		--exclude "*/compsl-${COMPSL_VERSION}/compsl-${COMPSL_VERSION}" \
		--exclude "config.opts" \
		--transform 's,^,compsl-${COMPSL_VERSION}/,' \
		-cjvf compsl-${COMPSL_VERSION}.tar.bz2 *

bin/dumper: src/dumper.o $(OBJECTS)
	@echo LINK $@
	@$(CC) $(ALL_CFLAGS) -MD $< $(OBJECTS) $(PLATLIBS) -o $@

bin/runner: src/runner.o $(OBJECTS)
	@echo LINK $@
	@$(CC) $(ALL_CFLAGS) -MD $< $(OBJECTS) $(PLATLIBS) -o $@

bin/perf-test: src/perf-test.o $(OBJECTS)
	@echo LINK $@
	@$(CC) $(ALL_CFLAGS) -MD $< $(OBJECTS) $(PLATLIBS) -o $@

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
	@printf "\n\nRUNNING TESTS\n"
	@printf "***************************************************\n"
statmsg:
	@printf "\nSETTINGS\n"
	@printf "***************************************************\n"
	@printf "$(STATMSG)\n"
	@printf "CFLAGS\n$(ALL_CFLAGS)\n\n" | fold -s
	@printf "PLATLIBS\n$(PLATLIBS)\n" | fold -s
	@sleep 5
	@printf "\nSTARTING OPERATION\n"
	@printf "***************************************************\n"
################################
# INTERNAL TARGETS             #
################################

#Dash makes it not error if not found
-include $(DEPS)
-include $(DDEPS)

src/include/intern/bcstrings.h: src/interp/bytecodes gen-bcstrings.sh Makefile
	@echo GEN $@
	@./gen-bcstrings.sh $< > $@

src/interp/jumptbl.h: src/interp/bytecodes gen-jumptbl.sh Makefile
	@echo GEN $@
	@./gen-jumptbl.sh $< > $@

src/include/compiler/bc_info.h: src/interp/bytecodes gen-bc-info.sh Makefile
	@echo GEN $@
	@./gen-bc-info.sh $< > $@

src/include/intern/bytecode.h: src/interp/bytecodes gen-bytecodeh.sh Makefile
	@echo GEN $@
	@./gen-bytecodeh.sh $< > $@

$(CMPATH)/compsl.tab.o: $(CMPATH)/compsl.tab.c $(GEN_HEADERS)
	@echo CC $<
	@$(CC) -MM -MQ $@ $(ALL_CFLAGS) $< > $(CMPATH)/compsl.tab.ddp
	@$(CC) -c  $(ALL_CFLAGS) -fno-gcse -Wno-unused-label $< -o $@

$(CMPATH)/lex.yy.o: $(CMPATH)/lex.yy.c $(GEN_HEADERS)
	@echo CC $<
	@$(CC) -MM -MQ $@ $(ALL_CFLAGS) $< > $(CMPATH)/lex.yy.ddp
	@$(CC) -c  $(ALL_CFLAGS) -fno-gcse -Wno-unused-label $< -o $@

#gcc manual says computed goto's may perform better with -fno-gcse
src/interp/run.o: src/interp/run.c config.mak Makefile $(GEN_HEADERS)
	@echo CC $<
	@$(CC) -MM -MG -MQ $@ $(ALL_CFLAGS) -Wno-unused-label $< -MF src/interp/run.dep
	@$(CC) -c  $(ALL_CFLAGS) -fno-gcse -falign-labels -Wno-unused-label $< -o $@

%.o: %.c config.mak Makefile $(GEN_HEADERS)
	@echo CC $<
	@$(CC) -MM -MG -MQ $@ $(ALL_CFLAGS) $< -MF $*.dep
	@$(CC) -c  $(ALL_CFLAGS) $< -o $@
	

#%.dep: %.c config.mak Makefile $(GEN_HEADERS)
#	@echo DEP $<
#	

ifeq ($(TARGET_WIN32),yes)
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
	@$(CC) $(LDFLAGS) $(OBJECTS) $(PLATLIBS) -o $(DYN_LIB_OUT) $(ALL_CFLAGS)
endif

$(STATIC_LIB_OUT): $(OBJECTS)
	@echo AR $@
	@ar rcs $(STATIC_LIB_OUT) $(OBJECTS)


################################
# FLEX/BISON TARGETS           #
################################

$(CMPATH)/compsl.tab.h: $(CMPATH)/compsl.y config.mak Makefile 
	@rm -f $(CMPATH)/compsl.tab.c $(CMPATH)/compsl.tab.h
	@echo BISON $<
	@$(BISON) --report all -d  $(CMPATH)/compsl.y -o $(CMPATH)/compsl.tab.c

$(CMPATH)/lex.yy.c: $(CMPATH)/compsl.l $(CMPATH)/compsl.tab.h config.mak Makefile $(GEN_HEADERS)
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

run-test-so: bin/test-so
	@echo
	@LD_LIBRARY_PATH="bin" $<
	@echo
bin/test-so: src/test/test-so.o $(DYN_LIB_OUT)
	@echo LINK $@
	@$(CC) $(ALL_CFLAGS) -MD $< $(PLATLIBS) -Lbin/ -lcompsl -o $@

####################################################

Makefile: setup.mk

ifeq ($(HAVE_CONFIG_OPTS),yes)
config.mak: configure config.opts
	@sh configure $(CONFIG_OPTS)
	@$(MAKE) clean
	@sleep 5
src/intern/config.h: config.mak
else
src/intern/config.h: configure
	@if [ -f config.mak ]; then \
	  echo "############################################################";\
	  echo "################## Please run ./configure ##################";\
	  echo "############################################################";\
	  exit 1;\
	else \
	  echo "############################################################";\
	  echo "####### Please run ./configure again - it's changed! #######";\
	  echo "############################################################";\
	fi
endif

ifeq ($(HAVE_CONFIG_OPTS),yes)
autoconfig: configure config.opts
	@sh configure $(CONFIG_OPTS)
	@$(MAKE) clean
	@sleep 5
else
autoconfig: configure
	@sh configure $(CONFIG_OPTS)
	@$(MAKE) clean
	@sleep 5
endif
