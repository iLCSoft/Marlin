########################################################
# cmake file for building Marlin example Package
# @author Jan Engels, Desy IT
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( mymarlin )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )


### SETTINGS ################################################################

# try to load ilcsoft CMakeModules from CMAKE_MODULE_PATH
INCLUDE( ilcsoft_cmake_modules )
IF( NOT ILCSOFT_CMAKE_MODULES_FOUND )
    MESSAGE( FATAL_ERROR "Could not find ilcsoft CMakeModules in CMAKE_MODULE_PATH" )
ENDIF( NOT ILCSOFT_CMAKE_MODULES_FOUND )
ILCSOFT_CMAKE_MODULES_MINIMUM_REQUIRED( 0.11 )
INCLUDE( ilcsoft_default_settings )



### DEPENDENCIES ############################################################

FIND_PACKAGE( Marlin 0.14 REQUIRED ) # minimum required Marlin version

INCLUDE_DIRECTORIES( ${Marlin_INCLUDE_DIRS} )
LINK_LIBRARIES( ${Marlin_LIBRARIES} )
#ADD_DEFINITIONS ( ${Marlin_DEFINITIONS} )


### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( doc )
ELSE()
    MESSAGE( STATUS "INSTALL_DOC set to OFF" )
    SET( INSTALL_DOC OFF )
ENDIF()



### LIBRARY #################################################################

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )
ADD_DEFINITIONS( "-Wno-long-long" )

# include directories
INCLUDE_DIRECTORIES( ./include )
INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

# add library
AUX_SOURCE_DIRECTORY( ./src library_sources )
ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${library_sources} )
INSTALL_SHARED_LIBRARY( ${PROJECT_NAME} DESTINATION lib )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

