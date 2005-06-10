#########################################################
#
# top level GNUmakefile to build Marlin with addtional 
# user packages containing processors
#
#  usage: all additional packages are expected to live 
#  in the packages subdirectory with the following structure:
#         ./packages/MyPackage/include
#                             ./src/
#                             ./src/GNUmakefile
#                             ./lib
#
#  where the GNUmakefile has a target 'lib; that creates
#  a library   ./packages/MyPackage/lib/libMyPackage.a  
#  see e.g.:  $MARLIN/examples/mymarlin
#  
#
# @author Frank Gaede
# @date   Jun,10 2005
#
#########################################################


#
# additional user libs go here 
#
# USERLIBS= -L path_to_lib -lmylib

USERLIBS=
export USERLIBS


subdirs := $(MARLIN) $(wildcard packages/*)  

packages := $(patsubst packages/%,%, $(wildcard packages/*) )

packagelibs := -Wl,-whole-archive $(foreach package,$(packages),  -L $(MARLIN)/packages/$(package)/lib -l$(package) ) -Wl,-no-whole-archive

export packagelibs


.PHONY: lib clean bin doc $(subdirs) test


test:
	@echo $(packages)
	@echo $(packagelibs)
	@for i in $(subdirs); do \
	echo "ls -l $$i" ; \
	ls -l $$i/lib ; done


all: lib bin 


lib:
	@for i in $(subdirs); do \
	echo "Building library for $$i..."; \
	(cd $$i/src; $(MAKE) $(MFLAGS) $(MYMAKEFLAGS) lib); done



clean:
	@for i in $(subdirs); do \
	echo "Clearing in $$i..."; \
	(cd $$i/src; $(MAKE) $(MFLAGS) $(MYMAKEFLAGS) clean); done

bin:
	$(MAKE) -C src bin

doc:
	$(MAKE) -C src doc
