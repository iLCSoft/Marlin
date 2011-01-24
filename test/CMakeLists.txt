##################################################################################
#
#    Marlin tests
#
##################################################################################

# build the marlintest plugin library
ADD_SUBDIRECTORY( ./marlintest )

# the actual tests  
ADD_SUBDIRECTORY( ./testmarlin )
