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


BINS = $(PROGNAME).cc


bin:  lib $(patsubst %.cc,$(BINDIR)/%,$(BINS))


$(BINDIR)/$(PROGNAME): $(PROGNAME).cc $(INSTALL)/lib$(PROGNAME).a
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(PROGNAME).cc  $(LIBS) 


#objects = $(patsubst %.cc,%.o,$(wildcard *.cc))

#objects = $(filter-out $(BINS), $(patsubst %.cc,%.o,$(wildcard *.cc))  )

objects = $(patsubst %.cc,%.o,$(filter-out $(BINS), $(wildcard *.cc))  )


$(INSTALL)/lib$(PROGNAME).a: $(objects)
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?

lib: $(INSTALL)/lib$(PROGNAME).a


clean:
	rm -f *.o $(INSTALL)/lib$(PROGNAME).a $(BINDIR)/$(PROGNAME)


