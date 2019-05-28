########################################################
# cmake file for building Marlin
# @author Jan Engels, Desy IT
CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
########################################################

# project name
PROJECT( Marlin )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 16 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )



### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES streamlog ILCTEST REQUIRED )
#FIND_PACKAGE( ILCSOFT_CMAKE_MODULES 1.11 REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )


# required packages
FIND_PACKAGE( Threads REQUIRED )
FIND_PACKAGE( LCIO REQUIRED )

# export Marlin_DEPENDS_INCLUDE_DIRS to MarlinConfig.cmake
SET( Marlin_DEPENDS_INCLUDE_DIRS ${LCIO_INCLUDE_DIRS} ${streamlog_INCLUDE_DIRS} )
SET( Marlin_DEPENDS_LIBRARY_DIRS ${LCIO_LIBRARY_DIRS} ${streamlog_LIBRARY_DIRS} )
SET( Marlin_DEPENDS_LIBRARIES ${LCIO_LIBRARIES} ${streamlog_LIBRARIES} )

INCLUDE_DIRECTORIES( SYSTEM ${Marlin_DEPENDS_INCLUDE_DIRS} )

OPTION( MARLIN_DD4HEP "Set to ON to build Marlin with DD4hep" ON )
IF( MARLIN_DD4HEP )
    FIND_PACKAGE( ROOT REQUIRED )
    FIND_PACKAGE( DD4hep COMPONENTS DDRec DDParsers )
    IF( NOT DD4hep_FOUND )
        MESSAGE( SEND_ERROR "DD4hep not found. Please set MARLIN_DD4HEP to OFF or DD4hep_DIR=/path/to/DD4hep" )
    ENDIF()
    IF( NOT ROOT_FOUND )
        MESSAGE( SEND_ERROR "ROOT not found. Please set MARLIN_DD4HEP to OFF or ROOT_DIR=/path/to/ROOT" )
    ENDIF()
ENDIF()

OPTION( MARLIN_GEAR "Set to ON to build Marlin with Gear" OFF )
IF( MARLIN_GEAR )
    FIND_PACKAGE( GEAR REQUIRED )
    IF( NOT GEAR_FOUND )
        MESSAGE( SEND_ERROR "GEAR not found. Please set MARLIN_GEAR to OFF or GEAR_DIR=/path/to/GEAR" )
    ENDIF()
ENDIF()


FOREACH( pkg GEAR DD4hep ROOT )
    IF( ${pkg}_FOUND )
        INCLUDE_DIRECTORIES( SYSTEM ${${pkg}_INCLUDE_DIRS} )
    ENDIF()
ENDFOREACH()


OPTION( MARLIN_GUI "Set to ON to build Marlin GUI" OFF )
MESSAGE( STATUS "MARLIN_GUI: ${MARLIN_GUI}" )
IF( MARLIN_GUI )

    # FindQt4 looks for qmake in $PATH
    IF( DEFINED QT4_DIR )
        SET( ENV{PATH} "${QT4_DIR}/bin:$ENV{PATH}" )
    ENDIF()

    FIND_PACKAGE( Qt4 )
    IF( NOT QT4_FOUND )
        MESSAGE( SEND_ERROR "QT4 not found. Please set MARLIN_GUI to OFF or QT4_DIR=/path/to/QT4" )
    ENDIF()

    ADD_DEFINITIONS ( -DQT_NO_DEBUG )
ENDIF()

### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC )

    FIND_PACKAGE( Doxygen )

    IF( DOXYGEN_FOUND )
        ADD_SUBDIRECTORY( ./doc )
    ELSE()
        MESSAGE( SEND_ERROR "Could not find doxygen required to build documentation" )
        MESSAGE( "Please install doxygen or set INSTALL_DOC to OFF" )
        MESSAGE( "" )
    ENDIF()

ENDIF()



### LIBRARY AND TOOLS #######################################################


# -------- generate MarlinConfig.h --------------------------------------------
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/MarlinConfig.h.cmake.in"
    "${PROJECT_BINARY_DIR}/marlin/MarlinConfig.h" @ONLY )

INSTALL( FILES "${PROJECT_BINARY_DIR}/marlin/MarlinConfig.h" DESTINATION "include/marlin/" )
INSTALL( FILES "${PROJECT_SOURCE_DIR}/source/python/compareMarlinSteeringFiles.py" DESTINATION "bin"
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
	      OWNER_READ GROUP_READ WORLD_READ
	      OWNER_WRITE )
# -----------------------------------------------------------------------------


ADD_SUBDIRECTORY( ./source )
##########################################################################
ADD_SUBDIRECTORY( ./test )


# display some variables and write them to cache
DISPLAY_STD_VARIABLES()


# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( MarlinConfig.cmake MarlinConfigVersion.cmake )
