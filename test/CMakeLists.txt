#########################
# CMakeLists.txt
#########################
cmake_minimum_required(VERSION 2.6)


# =========== helper macro to add test directory ==============================
MACRO( ADD_TEST_DIRECTORY subdir )

    FILE( RELATIVE_PATH curdir
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}
    ) 

    IF( curdir )
        SET( current_test ${curdir}_${subdir} )
    ELSE()
        SET( current_test ${subdir} )
    ENDIF()

    STRING( REPLACE "/" "_" current_test ${current_test} )

    OPTION( TEST_${current_test}
        "set to OFF to skip ${current_test}" ON )

    SET( current_test_dirname ${subdir} )
 
    IF( TEST_${current_test} )
        MESSAGE( STATUS "adding: ${current_test}" )
        ADD_SUBDIRECTORY( ${subdir} )
    ENDIF()

ENDMACRO()
# =============================================================================

# =========== helper macro to skip current test directory =====================
MACRO( SKIP_CURRENT_TEST )
    
    #SET( TEST_${current_test} CACHE BOOL
    #    "set to OFF to skip ${current_test}" FORCE )

    MESSAGE( STATUS "skipping: ${current_test}" )
ENDMACRO()
# =============================================================================

# =========== helper macro to add a standard ilctest ==========================
MACRO( ADD_STD_ILCTEST testname )

    INCLUDE_DIRECTORIES( "${ILCTest_SOURCE_DIR}/include" )
    AUX_SOURCE_DIRECTORY( . sources )

    IF( sources )
        ADD_EXECUTABLE( ${testname} ${sources} )
        ADD_TEST( t_${testname} "${CMAKE_CURRENT_BINARY_DIR}/${testname}" )
        SET_TESTS_PROPERTIES( t_${testname} PROPERTIES PASS_REGULAR_EXPRESSION "TEST_PASSED" )
        SET_TESTS_PROPERTIES( t_${testname} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED" )
    ELSE()
        MESSAGE( WARNING " ADD_STD_ILCTEST: no sources found in ${CMAKE_CURRENT_SOURCE_DIR}" )
    ENDIF()
    
ENDMACRO()


# =============================================================================


##################################################################################
#
#    the actual tests :
#
##################################################################################

# build the marlintest plugin
ADD_SUBDIRECTORY( marlintest )

# run some tests 
ADD_TEST_DIRECTORY( testmarlin )
