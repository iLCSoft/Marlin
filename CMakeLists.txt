########################################################
# cmake file for building Marlin
# @author Jan Engels, Desy IT
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( Marlin )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 14 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )
SET( ${PROJECT_NAME}_VERSION_TWEAK 0 )



### DEPENDENCIES ############################################################


# FindILCSOFT_CMAKE_MODULES.cmake needs to be in CMAKE_MODULE_PATH
FIND_PACKAGE( ILCSOFT_CMAKE_MODULES 0.11 REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )

# ------- required packages -------------------
SET( required_packages LCIO GEAR )

# ------- optional packages -------------------
SET( optional_packages CLHEP LCCD RAIDA )


# ------- do the search -----------------------
# fix for backwards compatibility
FOREACH( pkg ${required_packages} ${optional_packages} )
    IF( NOT ${pkg}_DIR AND DEFINED ${pkg}_HOME )
        SET( ${pkg}_DIR ${${pkg}_HOME} )
    ENDIF()
ENDFOREACH()

FOREACH( pkg ${required_packages} )
    FIND_PACKAGE( ${pkg} REQUIRED )
    INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
    LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
    ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
ENDFOREACH()

FOREACH( pkg ${optional_packages} )
    FIND_PACKAGE( ${pkg} QUIET )
    IF( ${pkg}_FOUND )
        INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
        ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
        MESSAGE( STATUS "${pkg}: ${${pkg}_DIR}" )
    ELSE()
        MESSAGE( STATUS "${pkg} NOT FOUND !!" )
    ENDIF()
ENDFOREACH()


SET( Marlin_DEPENDS_INCLUDE_DIRS ${LCIO_INCLUDE_DIRS} ${GEAR_INCLUDE_DIRS} )


### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )
#
#IF( INSTALL_DOC )
#
#    FIND_PACKAGE( Doxygen )
#    FIND_PACKAGE( LATEX )
#
#    IF( DOXYGEN_FOUND AND LATEX_COMPILER AND MAKEINDEX_COMPILER )
#        ADD_SUBDIRECTORY( ./doc )
#    ELSE()
#        IF( NOT DOXYGEN_FOUND )
#            MESSAGE( STATUS "Doxygen not found in your system!!" )
#        ENDIF()
#        IF( NOT LATEX_COMPILER OR NOT MAKEINDEX_COMPILER )
#            MESSAGE( STATUS "Latex not found in your system!!" )
#        ENDIF()
#        IF( INSTALL_DOC )
#            MESSAGE( STATUS "INSTALL_DOC set to OFF" )
#            SET( INSTALL_DOC OFF )
#        ENDIF()
#    ENDIF()
#ENDIF()
#

# DOCUMENTATION (FIXME)

FIND_PACKAGE( Doxygen )
IF( DOXYGEN_FOUND )

    ADD_CUSTOM_COMMAND(
        OUTPUT  "${PROJECT_SOURCE_DIR}/doc/html"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${PROJECT_SOURCE_DIR}/README" "${PROJECT_SOURCE_DIR}/doc/README"
        COMMAND "${DOXYGEN_EXECUTABLE}" ARGS "Doxyfile_cmake"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/doc"
        COMMENT "Building API Documentation..."
        VERBATIM )

    # add doc target
    ADD_CUSTOM_TARGET( doc DEPENDS
        "${PROJECT_SOURCE_DIR}/doc/html" )
ELSE()
    MESSAGE( STATUS "Doxygen not found in your system!!" )
    IF( INSTALL_DOC )
        MESSAGE( STATUS "INSTALL_DOC forced to OFF" )
        SET( INSTALL_DOC OFF )
    ENDIF()
ENDIF()

# install documentation
IF( INSTALL_DOC )
    # make sure doxygen is executed (make doc) before make install
    INSTALL( CODE "EXEC_PROGRAM(${CMAKE_BUILD_TOOL} ${PROJECT_BINARY_DIR} ARGS doc)" )
    # install documentation
    INSTALL_DIRECTORY( "${PROJECT_SOURCE_DIR}/doc" DESTINATION . )
ENDIF()

# create documentation overview.html
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/doc/overview.html.in"
                "${PROJECT_SOURCE_DIR}/doc/overview.html" @ONLY )






#############################################################################


# -------- generate MarlinConfig.h --------------------------------------------
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/MarlinConfig.h.cmake.in"
    "${PROJECT_BINARY_DIR}/marlin/MarlinConfig.h" @ONLY )

INSTALL( FILES "${PROJECT_BINARY_DIR}/marlin/MarlinConfig.h" DESTINATION "include/marlin/" )
# -----------------------------------------------------------------------------

# definitions
# FIXME rather add to CMAKE_CXX_Debug_FLAGS ...
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR
    CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    ADD_DEFINITIONS( "-DMARLINDEBUG" )
ENDIF()

ADD_DEFINITIONS( "-Wno-long-long" )

ADD_SUBDIRECTORY( ./streamlog )
ADD_SUBDIRECTORY( ./src )



# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

MESSAGE( STATUS "INSTALL_DOC = ${INSTALL_DOC}" )
MESSAGE( STATUS "MARLIN_GUI = ${MARLIN_GUI}" )
MESSAGE( STATUS )



# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( MarlinConfig.cmake MarlinConfigVersion.cmake MarlinLibDeps.cmake )

