######################################################
#
# GNUmakefile to build Marlin
#
# @author Frank Gaede
# @date   Jun,07 2004
######################################################

# name of the program

PROGNAME = Marlin

BASEDIR = ..



# check for private compiler definition
ifdef MY_CXX
  CXX = $(MY_CXX)
else
  CXX = g++
endif

AR = ar

INSTALL = $(BASEDIR)/lib
BINDIR = $(BASEDIR)/bin

INCLUDE =  -I $(LCIO)/src/cpp/include
INCLUDE +=  -I $(BASEDIR)/include


CPPFLAGS += $(INCLUDE)
CPPFLAGS += -g -Wall -ansi



LIBS =    -L $(LCIO)/lib -llcio -L $(LCIO)/sio/lib -lsio -lz
LIBS +=   -Wl,-whole-archive  -L $(INSTALL)  -l$(PROGNAME) -Wl,-no-whole-archive
LIBS +=  -Wl,--demangle


ifdef MARLIN_USE_AIDA
CPPFLAGS += -D MARLIN_USE_AIDA `aida-config --include`
LIBS += `aida-config --lib`
endif

BINS = $(PROGNAME).cc

LDFLAGS += --demangle


bin:  lib $(patsubst %.cc,$(BINDIR)/%,$(BINS))


$(BINDIR)/$(PROGNAME): $(PROGNAME).cc $(INSTALL)/lib$(PROGNAME).a
        mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(PROGNAME).cc  $(LIBS) 


objects = $(patsubst %.cc,%.o,$(filter-out $(BINS), $(wildcard *.cc))  )


$(INSTALL)/lib$(PROGNAME).a: $(objects)
        mkdir -p $(INSTALL)/lib
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?

lib: $(INSTALL)/lib$(PROGNAME).a


clean:
	rm -f *.o $(INSTALL)/lib$(PROGNAME).a $(BINDIR)/$(PROGNAME)



doc:	
	cd ../doc/ ; doxygen ;
