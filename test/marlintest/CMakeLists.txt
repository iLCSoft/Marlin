
### LIBRARY FOR RUNNING MARLIN TESTS ########################################

# library name
SET( libname marlintest )

# include directories
INCLUDE_DIRECTORIES( BEFORE ./include ${PROJECT_SOURCE_DIR}/source/include  ${PROJECT_BINARY_DIR} )
#INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

# add library
AUX_SOURCE_DIRECTORY( ./src library_sources )
ADD_SHARED_LIBRARY( ${libname} ${library_sources} )

TARGET_LINK_LIBRARIES( ${libname} Marlin )

#INSTALL_SHARED_LIBRARY( ${libname} DESTINATION lib )

FILE( GLOB MARLIN_UNIT_TESTS tests/*.cc )

FOREACH( utest ${MARLIN_UNIT_TESTS} )
  GET_FILENAME_COMPONENT( name ${utest} NAME_WE )
  ADD_EXECUTABLE( test_${name} ${utest} )
  TARGET_LINK_LIBRARIES( test_${name} Marlin )
  SET_TARGET_PROPERTIES( test_${name}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/bin"
  )
  ADD_TEST( test_${name}_t "${CMAKE_BINARY_DIR}/tests/bin/test_${name}" ) 
  SET_TESTS_PROPERTIES( test_${name}_t PROPERTIES PASS_REGULAR_EXPRESSION "TEST_PASSED" )
ENDFOREACH()