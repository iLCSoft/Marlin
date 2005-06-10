######################################################
#
# GNUmakefile to build Marlin
#
# @author Frank Gaede
# @date   Jun,07 2004
# 20050223 fg: added MARLINDEBUG, LCCD, CondDBMySQL
######################################################

# name of the program

PROGNAME = Marlin

BASEDIR := $(MARLIN)

# check for private compiler definition
ifdef MY_CXX
  CXX = $(MY_CXX)
else
  CXX = g++
endif

# debug/optimization level
ifdef MARLINDEBUG
 CPPFLAGS = -g -Wall -ansi
else
 CPPFLAGS = -O3 -Wall -ansi
endif



AR = ar

INSTALL = $(BASEDIR)/lib
BINDIR = $(BASEDIR)/bin

INCLUDE =  -I $(LCIO)/src/cpp/include
INCLUDE +=  -I $(BASEDIR)/include
#INCLUDE +=  -D TIXML_USE_STL=1 -I $(BASEDIR)/tinyxml

ifdef CLHEP
 INCLUDE += -D USE_CLHEP -I $(CLHEP)/include
endif

ifdef LCCD
 INCLUDE += -D USE_LCCD -I $(LCCD)/source/include
 ifdef CondDBMySQL
   INCLUDE += -D USE_CONDDB
 endif
endif


CPPFLAGS += $(INCLUDE)


LIBS =
LIBS +=   -Wl,-whole-archive  -L $(INSTALL)  -l$(PROGNAME) -Wl,-no-whole-archive
LIBS += $(packagelibs)

ifdef LCCD
 LIBS += -L $(LCCD)/lib -llccd
 ifdef CondDBMySQL
   LIBS += -L $(CondDBMySQL)/lib -lconddb    
 endif
endif

ifdef CLHEP
   LIBS += -L $(CLHEP)/lib -lCLHEP
endif


LIBS += $(USERLIBS)
LIBS +=    -L $(LCIO)/lib -llcio -L $(LCIO)/sio/lib -lsio -lz
LIBS +=  -Wl,--demangle

ifdef MARLIN_USE_AIDA
 ifneq ($(MARLIN_USE_AIDA),0) 
   CPPFLAGS += -D MARLIN_USE_AIDA `aida-config --include`
   LIBS += `aida-config --lib`
 endif
endif

#LIBS += -L $(BASEDIR)/tinyxml/lib -ltinyxml

BINS = $(PROGNAME).cc

LDFLAGS += --demangle


bin:  lib $(patsubst %.cc,$(BINDIR)/%,$(BINS))


$(BINDIR)/$(PROGNAME): $(PROGNAME).cc $(INSTALL)/lib$(PROGNAME).a
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(PROGNAME).cc  $(LIBS) 


objects = $(patsubst %.cc,%.o,$(filter-out $(BINS), $(wildcard *.cc))  )


$(INSTALL)/lib$(PROGNAME).a: $(objects)
	mkdir -p $(INSTALL)
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?

lib: $(INSTALL)/lib$(PROGNAME).a conf


clean:
	rm -f *.o $(INSTALL)/lib$(PROGNAME).a $(BINDIR)/$(PROGNAME)


doc:	
	cd ../doc/ ; doxygen ;


conf:
	echo "echo \""$(CPPFLAGS)"\"" > $(BINDIR)/marlin_includes.sh
	echo "echo \""$(LIBS)"\"" > $(BINDIR)/marlin_libs.sh

