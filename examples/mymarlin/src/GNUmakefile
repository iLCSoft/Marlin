######################################################
#
# Example GNUmakefile to build an Application with Marlin
#
# @author Frank Gaede
# @date   Dec, 02 2004
######################################################

# requires pathes to MARLIN and LCIO  !


#---------- user variables ---------------------------------------------------

# name of the program
PROGNAME = calomaptest


# the base directory of your application
# this is where you  would normaly have the lib, bin and source directories
BASEDIR = ..
INSTALL = $(BASEDIR)/lib
BINDIR = $(BASEDIR)/bin


#---------- user variables ---------------------------------------------------

ifdef MARLINDEBUG
 CPPFLAGS = -g -Wall -ansi
else
 CPPFLAGS = -O3 -Wall -ansi
endif


USERINCLUDES =
USERLIBS =


# check for private compiler definition
ifdef MY_CXX
  CXX = $(MY_CXX)
else
  CXX = g++
endif

AR = ar


#----- add libraries needed --------
ifdef MARLIN_USE_AIDA
USERINCLUDES += -D MARLIN_USE_AIDA `aida-config --include`
USERLIBS += `aida-config --lib`
endif

ifdef LCCD

 USERINCLUDES += -D USE_LCCD -I $(LCCD)/source/include
 USERLIBS += -L $(LCCD)/lib -llccd

 ifdef CondDBMySQL
   USERINCLUDES += -D USE_CONDDB
   USERLIBS += -L $(CondDBMySQL)/lib -lconddb    
 endif

endif



#--------------------------------------------------------------------------------------

LIBS +=   -Wl,-whole-archive  -L $(MARLIN)/lib  -lMarlin -Wl,-no-whole-archive
LIBS +=   -Wl,-whole-archive  -L $(INSTALL)  -l$(PROGNAME) -Wl,-no-whole-archive

#------ add additional libraries with marlin processors here, e.g.

#LIBS +=   -Wl,-whole-archive  -L $(CHECKPLOTS)/lib  -lCheckplots -Wl,-no-whole-archive



USERLIBS +=    -L $(LCIO)/lib -llcio -L $(LCIO)/sio/lib -lsio -lz
#--------------------------------------------------------------------------------------

INCLUDE =  -I $(LCIO)/src/cpp/include
INCLUDE +=  -I $(MARLIN)/include
INCLUDE +=  -I $(BASEDIR)/include

CPPFLAGS += $(INCLUDE)
CPPFLAGS += $(USERINCLUDES)
CPPFLAGS += -g -Wall -ansi

LIBS += $(USERLIBS)


bin:  lib $(BINDIR)/$(PROGNAME) $(BINDIR)/createmaps


BINS = createmaps.cc

$(BINDIR)/$(PROGNAME): $(INSTALL)/lib$(PROGNAME).a
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(MARLIN)/src/Marlin.cc $(LIBS) 

$(BINDIR)/createmaps: $(INSTALL)/lib$(PROGNAME).a createmaps.cc
	$(CXX) -o $(BINDIR)/createmaps $(CPPFLAGS) createmaps.cc $(LIBS) 


#objects = $(patsubst %.cc,%.o, $(wildcard *.cc)  )
objects = $(patsubst %.cc,%.o,$(filter-out $(BINS), $(wildcard *.cc))  )



$(INSTALL)/lib$(PROGNAME).a: $(objects)
	mkdir -p $(INSTALL)
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?


lib: $(INSTALL)/lib$(PROGNAME).a


clean:
	rm -f *.o $(INSTALL)/lib$(PROGNAME).a $(BINDIR)/$(PROGNAME)

doc:
	cd $(BASEDIR)/doc ; doxygen ;

