#########################################################
#
# top level GNUmakefile to build Marlin with additional
# user packages containing processors
#
# usage: all additional packages are expected to live
# in the packages subdirectory with the following structure:
#         $(MARLINWORKDIR)/packages/MyPackage/include
#         $(MARLINWORKDIR)/packages/MyPackage/src
#         $(MARLINWORKDIR)/packages/MyPackage/src/GNUmakefile
#
# see e.g.: $MARLIN/examples/mymarlin
#
#
# @author Frank Gaede, DESY
# @author Jan Engels, DESY
# @version $Id: GNUmakefile,v 1.14 2007-02-15 14:49:38 engels Exp $
#
#########################################################

# marlin working directory
ifndef MARLINWORKDIR
 MARLINWORKDIR=$(MARLIN)
 export MARLINWORKDIR
endif

# directories
PKGDIR = $(MARLINWORKDIR)/packages

# ---------------- additional user libs are defined in userlibs.gmk  -------------------

# if there is a userlib.gmk file in $MARLINWORKDIR include this one instead of the one in $MARLIN
USERLIBFILE = $(shell if [ -e $(MARLINWORKDIR)/userlib.gmk ] ; then echo ${MARLINWORKDIR}/userlib.gmk ; else echo ${MARLIN}/userlib.gmk ; fi)

-include $(USERLIBFILE)

#-------------------------------------------------------------------

# packages subdirs
subdirs := $(patsubst $(PKGDIR)/%/src/GNUmakefile,$(PKGDIR)/%, $(wildcard $(PKGDIR)/*/src/GNUmakefile))

.PHONY: all lib bin doc gui clean distclean

all: lib bin gui

lib:
	@$(MAKE) -C src lib
	@for i in $(subdirs); do \
	echo "********************************************************************************"; \
	echo "*   Building Marlin Package $$i ..."; \
	echo "********************************************************************************"; \
	$(MAKE) $(MFLAGS) $(MYMAKEFLAGS) -C $$i/src lib; done;

bin: lib
	@$(MAKE) -C src bin

gui: lib
	@$(MAKE) -C src gui

doc:
	@$(MAKE) -C src doc

clean:
	@$(MAKE) -C src clean
	@for i in $(subdirs); do \
	echo "********************************************************************************"; \
	echo "*   clearing $$i ..."; \
	echo "********************************************************************************"; \
	$(MAKE) -C $$i/src clean; done;

distclean:
	@$(MAKE) -C src distclean
	@for i in $(subdirs); do \
	echo "********************************************************************************"; \
	echo "*   clearing $$i ..."; \
	echo "********************************************************************************"; \
	$(MAKE) -C $$i/src distclean; done;
	rm -rf $(MARLINWORKDIR)/tmp

#end
