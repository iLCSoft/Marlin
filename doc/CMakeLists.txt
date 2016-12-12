########################################################
# cmake file for building Marlin documentation
# @author Jan Engels, Desy IT
########################################################

#FIND_PACKAGE( Doxygen REQUIRED ) # does not throw an ERROR if not found (fixed in cmake 2.8 )

# build documentation out-of-source
SET( DOC_SRC_DIR "${PROJECT_SOURCE_DIR}/doc" )
SET( DOC_BIN_DIR "${PROJECT_BINARY_DIR}/docbuild" )

SET( DOX_INPUT overview.html ../source ) 

# custom command to build documentation
ADD_CUSTOM_COMMAND(
    OUTPUT  "${DOC_BIN_DIR}/html/index.html" #"${DOC_BIN_DIR}/latex/index.tex"
    COMMAND DOX_PROJECT_NAME=${PROJECT_NAME}
            DOX_PROJECT_NUMBER="${${PROJECT_NAME}_VERSION}"
            DOX_OUTPUT_DIRECTORY="${DOC_BIN_DIR}"
            DOX_INPUT="${DOX_INPUT}"
            "${DOXYGEN_EXECUTABLE}"
    WORKING_DIRECTORY "${DOC_SRC_DIR}"
    COMMENT "Building API Documentation..."
    DEPENDS Doxyfile CMakeLists.txt
        ../source/include/marlin/*
        ../source/src/*
)

ADD_CUSTOM_TARGET( doc DEPENDS "${DOC_BIN_DIR}/html/index.html" ) # "${DOC_BIN_DIR}/latex/index.tex" )


# build documentation before 'make install'
INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} doc)" )


# in-source vs out-of-source installations
IF( CMAKE_INSTALL_PREFIX STREQUAL "${PROJECT_SOURCE_DIR}" )

    # --- in-source installations ---
    SET( DOC_INSTALL_PREFIX doc )

    # keep source tree clean
    ADD_CUSTOM_TARGET( uninstall_doc
        COMMAND "${CMAKE_COMMAND}" -E remove_directory "${CMAKE_INSTALL_PREFIX}/doc/html"
        COMMAND "${CMAKE_COMMAND}" -E remove "${CMAKE_INSTALL_PREFIX}/doc/README.md"
    )
    ADD_DEPENDENCIES( uninstall uninstall_doc )

ELSE()

    # --- out-of-source installations ---
    SET( DOC_INSTALL_PREFIX doc/${PROJECT_NAME} )

    INSTALL( FILES marlin_overview.gif release.notes  DESTINATION ${DOC_INSTALL_PREFIX} )

ENDIF( CMAKE_INSTALL_PREFIX STREQUAL "${PROJECT_SOURCE_DIR}" )


INSTALL( FILES "${PROJECT_SOURCE_DIR}/README.md" DESTINATION ${DOC_INSTALL_PREFIX} )

INSTALL( DIRECTORY "${DOC_BIN_DIR}/html" DESTINATION ${DOC_INSTALL_PREFIX} )

#----------------------------------------------------------------------------

