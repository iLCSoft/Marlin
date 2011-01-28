#
# define a few simple marlin test jobs 
#


ADD_TEST( t_marlinminusx "${EXECUTABLE_OUTPUT_PATH}/Marlin" -x ) 
SET_TESTS_PROPERTIES(t_marlinminusx PROPERTIES PASS_REGULAR_EXPRESSION "Example steering file for marlin" )
SET_TESTS_PROPERTIES( t_marlinminusx PROPERTIES PASS_REGULAR_EXPRESSION "<parameter name=\"RandomSeed\" value=\"1234567890\" />" )

#find the marlintest library and set the MARLIN_DLL to it
GET_TARGET_PROPERTY( MARLIN_DLL marlintest LOCATION )
MESSAGE( " using the following MARLIN_DLL for tests:"  ${MARLIN_DLL}  )



#---------------------------------------------------------------------------------------
SET( MARLIN_STEERING_FILE eventmodifier.xml )

SET( MARLIN_INPUT_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/${MARLIN_STEERING_FILE}
  ${CMAKE_CURRENT_SOURCE_DIR}/gear_simjob.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/simjob.slcio
)
CONFIGURE_FILE( runmarlin.cmake.in eventmodifier.cmake @ONLY ) 

ADD_TEST( t_eventmodifier cmake -P eventmodifier.cmake )
SET_TESTS_PROPERTIES( t_eventmodifier PROPERTIES PASS_REGULAR_EXPRESSION "TestEventModifier modified 3 events in 1 run" )


SET( MARLIN_STEERING_FILE processoreventseeder.xml )

SET( MARLIN_INPUT_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/${MARLIN_STEERING_FILE}
  ${CMAKE_CURRENT_SOURCE_DIR}/gear_simjob.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/simjob.slcio
)
CONFIGURE_FILE( runmarlin.cmake.in processoreventseeder.cmake @ONLY ) 

ADD_TEST( t_processoreventseeder cmake -P processoreventseeder.cmake )

SET_TESTS_PROPERTIES( t_processoreventseeder PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR .TestProcessorEventSeeder.* Seeds don't match"   )

SET_TESTS_PROPERTIES( t_processoreventseeder PROPERTIES PASS_REGULAR_EXPRESSION ". ERROR .TestProcessorEventSeeder.. ProcessorEventSeeder:registerProcessor" )

# the order of the match printed out on failure seems to go from last to first. So using the first of the two SET_TESTS_PROPERTIES below is more useful when trying to see when a specified test failed, in this case the test containing" Seeds don't match".  
#SET_TESTS_PROPERTIES( t_processoreventseeder PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR .TestProcessorEventSeeder.;ERROR .TestProcessorEventSeeder.* Seeds don't match" )
#SET_TESTS_PROPERTIES( t_processoreventseeder PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR .TestProcessorEventSeeder.* Seeds don't match;ERROR .TestProcessorEventSeeder."   )


#---------------------------------------------------------------------------------------
