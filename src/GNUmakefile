######################################################
#
# Makefile to build Marlin
#
# @author Frank Gaede
# @author Jan Engels
# @date   Jun,07 2004
# 20050223 fg: added MARLINDEBUG, LCCD, CondDBMySQL
######################################################

ifndef MARLINWORKDIR
 MARLINWORKDIR=$(MARLIN)
 export MARLINWORKDIR
endif

# name of the program
PROGNAME = Marlin

# name of the gui program
GUIPROGNAME = MarlinGUI

# directories
INCDIR = $(MARLIN)/include
LIBDIR = $(MARLIN)/lib
BINDIR = $(MARLINWORKDIR)/bin
PKGDIR = $(MARLINWORKDIR)/packages
DOCDIR = $(MARLINWORKDIR)/doc/$(PROGNAME)
TMPDIR = $(MARLINWORKDIR)/tmp/$(PROGNAME)
GUIDIR = $(MARLINWORKDIR)/tmp/$(GUIPROGNAME)
OBJDIR = $(TMPDIR)/obj
DEPDIR = $(TMPDIR)/dep

# source file for the binary
BINSRCS = $(PROGNAME).cc

# library file without path
LIBNAME = lib$(PROGNAME).a

# library file with path
LIB = $(LIBDIR)/$(LIBNAME)

# binary files with path
BIN = $(patsubst %.cc,$(BINDIR)/%,$(BINSRCS))

# object files for the binaries without path
BINOBJS = $(patsubst %.cc,%.o,$(BINSRCS))

# object files for the binaries with path
BINOBJ = $(patsubst %,$(OBJDIR)/%,$(BINOBJS))

# source files without path
SRCS = $(filter-out $(BINSRCS),$(wildcard *.cc))

# object files without path
OBJS = $(patsubst %.cc,%.o,$(SRCS))

# object files with path
OBJ = $(patsubst %,$(OBJDIR)/%,$(OBJS))

# dependency files without path
DEPS = $(patsubst %.o,%.d,$(OBJS) $(BINOBJS))

# dependency files with path
DEP = $(patsubst %,$(DEPDIR)/%,$(DEPS))

# package names (MarlinUtil MarlinReco CEDViewer...)
PKGNAMES = $(filter-out " CVS ", $(patsubst $(PKGDIR)/%,%,$(wildcard $(PKGDIR)/*)))

# package libraries with path
PKGLIBS = $(foreach pkg,$(PKGNAMES),$(PKGDIR)/$(pkg)/lib/lib$(pkg).a)

# package library flags (-LMarlinUtilLibPath -lMarlinUtil -LMarlinRecoLibPath -lMarlinReco ...)
PKGLIBFLAGS = $(foreach pkg,$(PKGNAMES), -L$(PKGDIR)/$(pkg)/lib -l$(pkg))

# package include flags (-IMarlinUtilPath/include -ICEDViewerPath/include)
PKGCPPFLAGS = $(foreach pkg,$(filter-out " MarlinReco ",$(PKGNAMES)), -I$(PKGDIR)/$(pkg)/include)

# c++ compiler
ifdef MY_CXX
 CXX = $(MY_CXX)
else
 CXX = g++
endif

# compiler flags
ifeq ($(MARLINDEBUG),1)
 CXXFLAGS = -g -Wall -ansi
else
 CXXFLAGS = -O3 -Wall -ansi
endif

# pre-processor flags
CPPFLAGS := -I$(INCDIR) -I$(LCIO)/src/cpp/include
CPPFLAGS += $(PKGCPPFLAGS)
CPPFLAGS += $(USERINCLUDES)

# libraries
LIBS = -Wl,-whole-archive
LIBS += -L$(LIBDIR) -l$(PROGNAME)
LIBS += $(PKGLIBFLAGS)
LIBS += -Wl,-no-whole-archive
LIBS += $(USERLIBS)

ifdef GEAR
 CPPFLAGS += -D USE_GEAR -I$(GEAR)/src/cpp/include
 LIBS += -L$(GEAR)/lib -lgearxml -lgear
endif

ifdef LCCD
 CPPFLAGS += -D USE_LCCD -I$(LCCD)/source/include
 LIBS += -L$(LCCD)/lib -llccd
 ifdef CondDBMySQL
  CPPFLAGS += -D USE_CONDDB
  LIBS += -L$(CondDBMySQL)/lib -lconddb
 endif
endif

ifdef CLHEP
 CPPFLAGS += -D USE_CLHEP -I$(CLHEP)/include
 LIBS += -L$(CLHEP)/lib -lCLHEP
endif

ifeq ($(MARLIN_USE_AIDA),1)
 CPPFLAGS += -D MARLIN_USE_AIDA `aida-config --include`
 LIBS += `aida-config --lib`
endif

LIBS += -L$(LCIO)/lib -llcio -L$(LCIO)/sio/lib -lsio -lz

# linker
LD = $(CXX)

# linker flags
LDFLAGS = -Wl,--demangle

# archiever
AR = ar cru

# check if a directory exists
DIR_EXISTS = test -d

.PHONY: all bin lib gui doc conf clean distclean

all: lib bin gui

ifeq ($(MARLIN_GUI),1)
ifndef QTDIR
gui:
	@echo "Warning: $(GUIPROGNAME) will not be built!!" ;
	@echo "Problems were found with the installation of QT, see $(MARLIN)/gui/README for details on how to install QT..." ;
else
gui: lib $(BINDIR)/$(GUIPROGNAME)
endif
else
gui:
	@echo "Warning: $(GUIPROGNAME) will not be built!!" ;
	@echo "To build $(GUIPROGNAME) set the environment variable MARLIN_GUI to 1" ;
endif

bin: conf lib $(BINOBJ) $(BIN)
	
binonly: conf $(BINOBJ)
	@$(DIR_EXISTS) $(BINDIR) || mkdir -p $(BINDIR)
	$(LD) $(BINOBJ) $(LDFLAGS) $(LIBS) -o $(BIN)

lib: conf $(LIB)

doc:
	@$(DIR_EXISTS) $(DOCDIR) || mkdir -p $(DOCDIR) ;
	@cp -f $(MARLIN)/README $(MARLIN)/doc/release.notes \
	$(MARLIN)/doc/overview.html $(MARLIN)/doc/marlin_overview.gif $(DOCDIR) ;
	@export DOX_PROGNAME=$(PROGNAME) ; \
	export DOX_DOCDIR=$(DOCDIR) ; \
	cd ../doc ; doxygen ;

conf:
	@$(DIR_EXISTS) $(BINDIR) || mkdir -p $(BINDIR)
	@echo "echo \""$(CPPFLAGS)"\"" > $(BINDIR)/marlin_includes.sh
	@echo "echo \""$(LIBS)"\"" > $(BINDIR)/marlin_libs.sh
	@chmod 755 $(BINDIR)/marlin_includes.sh $(BINDIR)/marlin_libs.sh

clean:
	@if [ -f "$(GUIDIR)/gui/Makefile" ] ; then \
	echo "Clearing $(GUIPROGNAME)..."; \
	(cd $(GUIDIR)/gui; $(MAKE) distclean); fi ;
	rm -f $(OBJ) $(BINOBJ) $(BIN) $(LIB)

distclean:
	@if [ -f "$(GUIDIR)/gui/Makefile" ] ; then \
	echo "Clearing $(GUIPROGNAME)..."; \
	(cd $(GUIDIR)/gui; $(MAKE) distclean); fi ;
	rm -rf $(BIN) $(LIB) $(TMPDIR) $(BINDIR)/marlin_includes.sh $(BINDIR)/marlin_libs.sh $(DOCDIR)

# tell make to include dependencies
-include $(DEP)

# marlin gui rule
$(BINDIR)/$(GUIPROGNAME): $(BIN)
	@if [ -f "$(QTDIR)/bin/qmake" ] ; then \
	if [ ! -d "$(GUIDIR)/gui" ] ; then \
	echo "Copying $(GUIPROGNAME) sources from [$(MARLIN)/gui] to [$(GUIDIR)/gui]" ; \
	$(DIR_EXISTS) $(GUIDIR) || mkdir -p $(GUIDIR) ; \
	cp -rf $(MARLIN)/gui $(GUIDIR) ; fi ; \
	echo "Building $(GUIPROGNAME)..."; \
	rm -f $(BINDIR)/$(GUIPROGNAME) ; \
	cd "$(GUIDIR)/gui"; qmake ; $(MAKE) ; fi

# binary rule
$(BIN) : $(OBJ) $(BINOBJ) $(PKGLIBS)
	@$(DIR_EXISTS) $(BINDIR) || mkdir -p $(BINDIR)
	$(LD) $(OBJDIR)/$(notdir $@).o $(LDFLAGS) $(LIBS) -o $@

# objects rule
$(OBJDIR)/%.o: %.cc
	@$(DIR_EXISTS) $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# library rule
$(LIB) : $(OBJ)
	@$(DIR_EXISTS) $(LIBDIR) || mkdir -p $(LIBDIR)
	$(AR) $@ $?

# dependency rule
$(DEPDIR)/%.d : %.cc
	@$(DIR_EXISTS) $(DEPDIR) || mkdir -p $(DEPDIR)
	@echo "Creating dependencies for $<" ; \
	set -e; $(CXX) -MM $(CPPFLAGS) $< \
	| sed 's|\($*\)\.o[ :]*|$(OBJDIR)/\1.o $(DEPDIR)/$*.d : |g'\
	> $@; [ -s $@ ] || rm -f $@

# end
