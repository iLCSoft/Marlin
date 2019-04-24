
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



function ( MARLIN_ADD_TEST test_name )
  cmake_parse_arguments(ARG "BUILD_EXEC" "" "COMMAND;DEPENDS;EXEC_ARGS;REGEX_PASS;REGEX_PASSED;REGEX_FAIL;REGEX_FAILED;REQUIRES" ${ARGN} )
  set ( missing )
  set ( use_test 1 )

  if ( ${ARG_BUILD_EXEC} )
    add_executable ( ${test_name} main/${test_name}.cc )
    target_link_libraries( ${test_name} Marlin )
    target_include_directories( ${test_name} PUBLIC include )
    set_target_properties( ${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )
  endif()

  set ( cmd ${ARG_COMMAND} )
  if ( "${cmd}" STREQUAL "" )
    if( ${ARG_BUILD_EXEC} )
      set ( cmd ${CMAKE_BINARY_DIR}/bin/${test_name} )
    else()
      message( SEND_ERROR "No command no executable provided" )
    endif()
  endif()

  set ( passed ${ARG_REGEX_PASS} ${ARG_REGEX_PASSED} )
  if ( "${passed}" STREQUAL "NONE" )
    unset ( passed )
  elseif ( "${passed}" STREQUAL "" )
    set ( passed "TEST_PASSED" )
  endif()

  set ( failed ${ARG_REGEX_FAIL} ${ARG_REGEX_FAILED} )
  if ( "${failed}" STREQUAL "NONE" )
    unset ( failed )
  endif()

  set ( args ${ARG_EXEC_ARGS} )
  # if ( "${args}" STREQUAL "" )
  #   set ( args ${test_name} )
  # endif()
  add_test(NAME t_${test_name} COMMAND ${cmd} ${args} )
  if ( NOT "${passed}" STREQUAL "" )
    set_tests_properties( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${passed}" )
  endif()
  if ( NOT "${failed}" STREQUAL "" )
    set_tests_properties( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${failed}" )
  endif()
  # Set test dependencies if present
  foreach ( _dep ${ARG_DEPENDS} )
    set_tests_properties( t_${test_name} PROPERTIES DEPENDS t_${_dep} )
  endforeach()
endfunction()

MARLIN_ADD_TEST ( test-thread-pool BUILD_EXEC REGEX_FAIL "TEST_FAILED" )

#INSTALL_SHARED_LIBRARY( ${libname} DESTINATION lib )
