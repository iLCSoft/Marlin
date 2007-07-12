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
PROGNAME = MyMarlin





# the base directory of your application
# this is where you  would normaly have the lib, bin and source directories
BASEDIR = ..
INSTALL = $(BASEDIR)/lib
BINDIR = $(BASEDIR)/bin

#---------- user variables ---------------------------------------------------
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




#--------------------------------------------------------------------------------------

LIBS =    -L $(LCIO)/lib -llcio -L $(LCIO)/sio/lib -lsio -lz
LIBS +=   -Wl,-whole-archive  -L $(MARLIN)/lib  -lMarlin -Wl,-no-whole-archive
LIBS +=   -Wl,-whole-archive  -L $(INSTALL)  -l$(PROGNAME) -Wl,-no-whole-archive

#------ add additional libraries with marlin processors here, e.g.

#LIBS +=   -Wl,-whole-archive  -L $(CHECKPLOTS)/lib  -lCheckplots -Wl,-no-whole-archive



#--------------------------------------------------------------------------------------

INCLUDE =  -I $(LCIO)/src/cpp/include
INCLUDE +=  -I $(MARLIN)/include
INCLUDE +=  -I $(BASEDIR)/include

CPPFLAGS += $(INCLUDE)
CPPFLAGS += $(USERINCLUDES)
CPPFLAGS += -g -Wall -ansi

LIBS += $(USERLIBS)


bin:  lib $(BINDIR)/$(PROGNAME)


$(BINDIR)/$(PROGNAME): $(INSTALL)/lib$(PROGNAME).a
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(MARLIN)/src/Marlin.cc $(LIBS) 


objects = $(patsubst %.cc,%.o, $(wildcard *.cc)  )


$(INSTALL)/lib$(PROGNAME).a: $(objects)
	mkdir -p $(INSTALL)/lib
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?


lib: $(INSTALL)/lib$(PROGNAME).a


clean:
	rm -f *.o $(INSTALL)/lib$(PROGNAME).a $(BINDIR)/$(PROGNAME)

doc:
	cd $(BASEDIR)/doc ; doxygen ;

