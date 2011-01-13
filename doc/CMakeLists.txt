########################################################
# cmake file for building Marlin documentation
# @author Jan Engels, Desy IT
########################################################

#FIND_PACKAGE( Doxygen REQUIRED ) # does not throw an ERROR if not found (fixed in cmake 2.8 )

# build documentation out-of-source
SET( DOC_SRC_DIR "${PROJECT_SOURCE_DIR}/doc" )
SET( DOC_BIN_DIR "${PROJECT_BINARY_DIR}/docbuild" )

SET( DOXYFILE_INPUT "overview.html ${PROJECT_SOURCE_DIR}/source/include/marlin ${PROJECT_SOURCE_DIR}/source/src ${PROJECT_SOURCE_DIR}/streamlog/source" )

CONFIGURE_FILE( "${DOC_SRC_DIR}/Doxyfile.cmake.in" "${DOC_BIN_DIR}/Doxyfile" @ONLY )
CONFIGURE_FILE( "${DOC_SRC_DIR}/overview.html.in" "${DOC_BIN_DIR}/overview.html" @ONLY )


ADD_CUSTOM_COMMAND(
    OUTPUT "${DOC_BIN_DIR}/html/index.html" "${DOC_BIN_DIR}/latex/index.tex"
    # copy over doc from source tree to keep source tree clean
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${DOC_SRC_DIR}" "${DOC_BIN_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${PROJECT_SOURCE_DIR}/README" "${DOC_BIN_DIR}"
    #COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/example" "${DOC_BIN_DIR}/example"
    COMMAND "${DOXYGEN_EXECUTABLE}"
    WORKING_DIRECTORY "${DOC_BIN_DIR}"
    COMMENT "Building API Documentation..."
    DEPENDS "${DOC_SRC_DIR}/overview.html.in" "${DOC_SRC_DIR}/Doxyfile.cmake.in"
)

ADD_CUSTOM_TARGET( doc DEPENDS
    "${DOC_BIN_DIR}/html/index.html"
    #"${DOC_BIN_DIR}/latex/index.tex"
)

# build documentation before 'make install'
INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} doc)" )


# in-source vs out-of-source installations
IF( CMAKE_INSTALL_PREFIX STREQUAL "${PROJECT_SOURCE_DIR}" )

    # --- in-source installations ---
    INSTALL_DIRECTORY( "${DOC_BIN_DIR}/html" DESTINATION "doc" )
    #INSTALL_DIRECTORY( "${DOC_BIN_DIR}/example" DESTINATION "doc" )

    # keep source tree clean
    #ADD_CUSTOM_TARGET( uninstall_doc
    #    COMMAND "${CMAKE_COMMAND}" -E remove_directory "${CMAKE_INSTALL_PREFIX}/doc/html"
    #    COMMAND "${CMAKE_COMMAND}" -E remove_directory "${CMAKE_INSTALL_PREFIX}/doc/example"
    #)
    #ADD_DEPENDENCIES( uninstall uninstall_doc )


ELSE()

    # --- out-of-source installations ---
    INSTALL_DIRECTORY( "${DOC_BIN_DIR}/" DESTINATION "doc/marlin"
        PATTERN "*Doxyfile*" EXCLUDE
        PATTERN "*latex*" EXCLUDE
        PATTERN "*manual*" EXCLUDE
        PATTERN "*overview.html*" EXCLUDE
        PATTERN "*.txt" EXCLUDE
    )

ENDIF( CMAKE_INSTALL_PREFIX STREQUAL "${PROJECT_SOURCE_DIR}" )

#----------------------------------------------------------------------------

