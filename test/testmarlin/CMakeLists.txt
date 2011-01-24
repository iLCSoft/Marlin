#
# define a few simple marlin test jobs 
#
            

ADD_TEST( t_marlinminusx "${EXECUTABLE_OUTPUT_PATH}/Marlin" -x ) 
SET_TESTS_PROPERTIES(t_marlinminusx PROPERTIES PASS_REGULAR_EXPRESSION "Example steering file for marlin" )

CONFIGURE_FILE( eventmodifier.cmake.in eventmodifier.cmake @ONLY ) 
ADD_TEST( t_eventmodifier cmake -P eventmodifier.cmake )
SET_TESTS_PROPERTIES( t_eventmodifier PROPERTIES PASS_REGULAR_EXPRESSION "TestEventModifier modified 3 events in 1 run" )
