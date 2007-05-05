# this file sets up variables further, based on the values from config.mak
-include config.mak

CFLAGS  = -ftabstop=4 -Wall -Wbad-function-cast -Wcast-align -Wwrite-strings
CFLAGS += -Wnonnull
#CFLAGS += -Wunreachable-code

override CPUFLAGS =
ifeq ($(TARGET_MMX),yes)
    override CPUFLAGS += -mmmx
endif
ifeq ($(TARGET_SSE2),yes)
    override CPUFLAGS += -msse2 -mfpmath=sse,387
else
    ifeq ($(TARGET_SSE),yes)
        override CPUFLAGS += -msse -mfpmath=sse,387 
    endif
endif

override APPSTATMSG=
DEFFLAGS=
ifeq ($(DEBUG_COMP),yes)
	DEFFLAGS += -DDEBUG_COMP
	override APPSTATMSG += Compiler Debug messages are ON\\n
else
	override APPSTATMSG += Compiler Debug messages are OFF\\n
endif
ifeq ($(TRACE_INTERP),yes)
	DEFFLAGS += -D_COMPSL_TRACE
	override APPSTATMSG += Interpreter execution tracing is ON\\n
else
	override APPSTATMSG += Interpreter execution tracing is OFF\\n
endif

#cause the optimizer to output code dumps as it runs each stage
ifeq ($(TRACE_OPTIM),yes)
	DEFFLAGS += -DCOMPSL_TRACE_OPTIM
	override APPSTATMSG += Tracing bytecode optimizer is ON\\n
else
	override APPSTATMSG += Tracing bytecode optimizer is OFF\\n
endif

# make the compiler output the line number it's currently compiling 
# to stdout
ifeq ($(TRACE_COMPILE),yes)
	DEFFLAGS += -DCOMPSL_TRACE_COMPILE
	override APPSTATMSG += Tracing compile is ON\\n
else
	override APPSTATMSG += Tracing compile is OFF\\n
endif

ifeq ($(STACK_CHECK),yes)
	DEFFLAGS += -DCOMP_STACKCHECK
	override APPSTATMSG += Compile time bytecode stack bounds checking is ON\\n
else
	override APPSTATMSG += Compile time bytecode stack bounds checking is OFF\\n
endif

ifdef DEBUG
	# -fmudflap is some pointer debuging stuff
	# -fno-builtins since we can't set breakpoints on calls to builtin functions
	# -fstack-protector checks for stack overruns
	CFLAGS += -fmudflap -fno-builtin -fstack-protector
	OPTIMIZE=0
else
	DEFFLAGS += -DNDEBUG
	OPTIMIZE=FULL
endif

ifeq ($(OPTIMIZE),FULL)
	CFLAGS += -O3 -frename-registers 
	CFLAGS += -funit-at-a-time -funroll-loops -finline-functions -funswitch-loops
	CFLAGS += -fsched-spec-load -maccumulate-outgoing-args
	CFLAGS += -minline-all-stringops -fomit-frame-pointer
	CFLAGS += -finline-limit=2000
	CFLAGS += -fno-stack-limit
	
	#CFLAGS +=-fdata-sections -ffunction-sections
	# TODO: make sure none of these breaks the library for linking....
	CFLAGS += -fstrict-aliasing -Wstrict-aliasing=2 
	
	CFLAGS += -fgcse-sm -fgcse-las -fgcse-after-reload
	CFLAGS += -ftree-vectorize
	CFLAGS += -ftracer
	CFLAGS += -fvariable-expansion-in-unroller
	#CFLAGS += -fprefetch-loop-arrays
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

CFLAGS += -fsingle-precision-constant -fno-math-errno 
CFLAGS += -ffast-math -mno-ieee-fp
CFLAGS += -ffinite-math-only -fno-trapping-math 

override CFLAGS := $(shell CC=$(CC) ./gcc-optioncheck $(CFLAGS))

MYCFLAGS := -fbuiltin -D_GNU_SOURCE -DBUILDING_COMPSL -Wno-attributes

# for shared library
ifneq ($(TARGET_WIN32),yes)
	MYCFLAGS += -fvisibility=hidden
	DEFFLAGS += -DWINDOWS
endif

MYCFLAGS := $(shell CC=$(CC) ./gcc-optioncheck $(MYCFLAGS))
ALL_CFLAGS := -std=gnu99 $(OPTFLAGS) $(CPUFLAGS) $(MYCFLAGS) $(CFLAGS) $(DEFFLAGS)


STATMSG  = Compiling with $(CC) version $(shell $(CC) -dumpversion)
STATMSG += targeting $(TARGET_CPU) on $(TARGET_OS) for
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
ifeq ($(CPUGUESS),yes)
    STATMSG += Guessed CPUTYPE of $(GUESSED_CPU)\\n
    STATMSG +=    Adding CPU flags $(CPUFLAGS)\\n
else
    STATMSG += Compiling for $(TARGET_CPU) with $(CPUFLAGS)\\n
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
STATMSG +=datadir     = $(datadir)\\n
STATMSG +=sysconfdir  = $(sysconfdir)\\n
STATMSG +=includedir  = $(includedir)\\n
STATMSG +=docdir      = $(docdir)\\n
STATMSG +=libdir      = $(libdir)\\n
STATMSG +=mandir      = $(mandir)\\n

STATMSG +=\\n