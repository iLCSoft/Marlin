########################################################
# cmake file for building Marlin example documentation
# @author Jan Engels, DESY
########################################################


FIND_PACKAGE( Doxygen )

IF( DOXYGEN_FOUND )

    # build documentation out-of-source
    SET( DOC_SRC_DIR "${PROJECT_SOURCE_DIR}/doc" )
    SET( DOC_BIN_DIR "${PROJECT_BINARY_DIR}/docbuild" )

    ADD_CUSTOM_COMMAND(
        OUTPUT  "${DOC_BIN_DIR}/html/index.html"
        # copy over doc from source tree to keep source tree clean
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${DOC_SRC_DIR}" "${DOC_BIN_DIR}"
        COMMAND "${DOXYGEN_EXECUTABLE}"
        WORKING_DIRECTORY "${DOC_BIN_DIR}"
        COMMENT "Building API Documentation..."
        DEPENDS "${DOC_SRC_DIR}/Doxyfile.cmake.in"
    )

    # add doc target
    ADD_CUSTOM_TARGET( doc DEPENDS "${DOC_BIN_DIR}/html/index.html" )

    # replace INPUT = @PROJECT_SOURCE_DIR@/{src,include}
    CONFIGURE_FILE( "${DOC_SRC_DIR}/Doxyfile.cmake.in" "${DOC_BIN_DIR}/Doxyfile" @ONLY )

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
