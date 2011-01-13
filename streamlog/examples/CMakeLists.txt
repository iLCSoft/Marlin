##################################
# cmake file for building streamlog binaries
# @author Frank Gaede, DESY
##################################

# include directories
#INCLUDE_DIRECTORIES( ../source/include )

# list of test binaries
SET( test_bins test_log)

# create library with example classes 
ADD_LIBRARY( streamlog_test EXCLUDE_FROM_ALL fibonacci.cc  )

# create symbolic bin target
ADD_CUSTOM_TARGET( tests )

# loop through list of test binaries
FOREACH( bin ${test_bins} )
    ADD_EXECUTABLE( ${bin} EXCLUDE_FROM_ALL ./${bin}.cc )
    ADD_DEPENDENCIES( tests ${bin} )
    TARGET_LINK_LIBRARIES( ${bin} streamlog streamlog_test )
    #INSTALL( TARGETS ${bin} DESTINATION bin/tests )
ENDFOREACH()

