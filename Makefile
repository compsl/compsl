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

CC=gcc
CFLAGS=-ftabstop=4 -Wall -Wextra -Wfloat-equal -Wbad-function-cast -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-noreturn -Wunreachable-code -fPIC -std=gnu99 -mmmx

ifdef DEBUG
	CFLAGS += -ggdb -D DEBUG
else
	CFLAGS += -O2 -fdata-sections -ffunction-sections -fbranch-target-load-optimize -frename-registers -fweb -ffast-math -fsingle-precision-constant -funroll-loops -finline-functions -fsched-spec-load -fsched2-use-superblocks -fmove-all-movables
	# TODO: figure out if we need the -fno-strict-aliasing option.
	# TODO: make sure none of these breaks the library for linking....
endif

SOURCES := src/compartment.c  src/error.c  src/gen.c  src/run.c  src/vars.c src/vm.c src/compiler/lex.yy.c src/compiler/compsl.tab.c 
OBJECTS := $(SOURCES:.c=.o)
DEPS := $(SOURCES:.c=.d)
TESTSRCS := src/test/test-interp.c
TESTOBJS := $(TESTSRCS:.c=.o)


CMPLRPATH=src/compiler

SHORTLIB=compsl
LIBNAME := lib$(SHORTLIB)

TEST_EXE=bin/interp-test
CMPRL_TEST_EXE=bin/compsl-cmplr
STATIC_LIB_OUT := bin/$(LIBNAME).a
DYN_LIB_OUT := bin/$(LIBNAME).so.1.0.1



#TARGETS
all: compile static dynamic maketestonly
	         
compile: $(SOURCES) $(OBJECTS)

static: compile $(STATIC_LIB_OUT)
dynamic: compile $(DYN_LIB_OUT)

test: maketestonly
	bash bin/test-driver $(TEST_EXE)
	
# make test executibles, assumes that all tests are single object/source file
# linked to libcompsl.a
maketestonly: $(TESTOBJS) static
	for obj in $(TESTOBJS); do \
		$(CC) -static $$obj -Lbin -l$(SHORTLIB) -o bin/$$(basename $$obj .o); \
	done

clean:
	rm -f $(OBJECTS) $(STATIC_LIB_OUT) $(DYN_LIB_OUT) $(TEST_EXE) $(CMPRL_TEST_EXE) $(DEPS)



# FLEX/BISON TARGETS
$(CMPLRPATH)/lex.yy.c: $(CMPLRPATH)/compsl.l 
	flex -o$@ $<

$(CMPLRPATH)/compsl.tab.c $(CMPLRPATH)/compsl.tab.h: $(CMPLRPATH)/compsl.y
	bison -d $< -o $@


# INTERNAL TARGETS


include $(DEPS)

$(DEPS): $(SOURCES)
	$(CC) $(CFLAGS) -MM -MG $< -MF $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

$(STATIC_LIB_OUT):
	ar rcs $(STATIC_LIB_OUT) $(OBJECTS)

$(DYN_LIB_OUT): 
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.1 -o $(DYN_LIB_OUT)  $(OBJECTS)









