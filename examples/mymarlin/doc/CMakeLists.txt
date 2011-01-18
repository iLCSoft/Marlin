########################################################
# cmake file for building Marlin example documentation
# @author Jan Engels, DESY
########################################################


FIND_PACKAGE( Doxygen )

IF( DOXYGEN_FOUND )

    # build documentation out-of-source
    SET( DOC_SRC_DIR "${PROJECT_SOURCE_DIR}/doc" )
    SET( DOC_BIN_DIR "${PROJECT_BINARY_DIR}/docbuild" )
    
    # directories to search for documentation
    SET( DOX_INPUT ../include ../src )


    # custom command to build documentation
    ADD_CUSTOM_COMMAND(
        OUTPUT  "${DOC_BIN_DIR}/html/index.html"
        COMMAND DOX_PROJECT_NAME=${PROJECT_NAME}
                DOX_PROJECT_NUMBER="${${PROJECT_NAME}_VERSION}"
                DOX_OUTPUT_DIRECTORY="${DOC_BIN_DIR}"
                DOX_INPUT="${DOX_INPUT}"
                "${DOXYGEN_EXECUTABLE}"
        WORKING_DIRECTORY "${DOC_SRC_DIR}"
        COMMENT "Building API Documentation..."
        DEPENDS Doxyfile CMakeLists.txt ../include/* ../src/*
    )

    # add doc target
    ADD_CUSTOM_TARGET( doc DEPENDS "${DOC_BIN_DIR}/html/index.html" )

ELSE()

    MESSAGE( STATUS "Doxygen not found -- INSTALL_DOC set to OFF" )
    SET( INSTALL_DOC OFF )

ENDIF()



IF( INSTALL_DOC )

    # make sure doxygen is executed (make doc) before make install
    INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} doc)" )

    # in-source vs out-of-source installations
    IF( CMAKE_INSTALL_PREFIX STREQUAL "${PROJECT_SOURCE_DIR}" )
        # --- in-source installations ---
        INSTALL( DIRECTORY "${DOC_BIN_DIR}/html" DESTINATION "doc" )
    ELSE()
        # --- out-of-source installations ---
        INSTALL( DIRECTORY "${DOC_BIN_DIR}/html" DESTINATION "doc/${PROJECT_NAME}" )
    ENDIF()

ENDIF()
