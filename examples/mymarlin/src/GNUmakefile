######################################################
#
# Example GNUmakefile to build an Application with Marlin
#
# @author Frank Gaede
# @date   Dec, 02 2004
######################################################

# requires pathes to MARLIN and Marlin v00-09 or higher
# can be called from top level makefile

#---------- user variables ---------------------------------------------------

# name of the program/library
# fg20050610: renamed to mymarlin in order to have same name as directory 
PROGNAME = mymarlin


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


#----  additional user include/libraries
USERINCLUDES = 
USERLIBS =
#--------------------------------------


# check for private compiler definition
ifdef MY_CXX
  CXX = $(MY_CXX)
else
  CXX = g++
endif

AR = ar


#--------------------------------------------------------------------------------------

LIBS +=   -Wl,-whole-archive  -L $(INSTALL)  -l$(PROGNAME) -Wl,-no-whole-archive

#------ add additional libraries with marlin processors here, e.g.

#LIBS +=   -Wl,-whole-archive  -L $(CHECKPLOTS)/lib  -lCheckplots -Wl,-no-whole-archive

#--------------------------------------------------------------------------------------

INCLUDE +=  -I $(BASEDIR)/include
INCLUDE += $(USERINCLUDE)
 
CPPFLAGS += $(INCLUDE)

CPPFLAGS += `. $(MARLIN)/bin/marlin_includes.sh`

LIBS += $(USERLIBS)
LIBS += `. $(MARLIN)/bin/marlin_libs.sh`



bin:  lib $(BINDIR)/$(PROGNAME)

$(BINDIR)/$(PROGNAME): $(INSTALL)/lib$(PROGNAME).a
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(MARLIN)/src/Marlin.cc $(LIBS) 

objects = $(patsubst %.cc,%.o, $(wildcard *.cc)  )

$(INSTALL)/lib$(PROGNAME).a: $(objects)
	mkdir -p $(INSTALL)
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?

lib: $(INSTALL)/lib$(PROGNAME).a


clean:
	rm -f *.o $(INSTALL)/lib$(PROGNAME).a $(BINDIR)/$(PROGNAME)

doc:
	cd $(BASEDIR)/doc ; doxygen ;

