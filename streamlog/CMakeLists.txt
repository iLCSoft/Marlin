#################################
# cmake file for building streamlog
# @author Frank Gaede, DESY
#################################

# cmake minimum required version
CMAKE_MINIMUM_REQUIRED( VERSION 2.4 )
SET( CMAKE_BACKWARDS_COMPATIBILITY 2.2 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

# allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

# project name
PROJECT( streamlog )

# project options
OPTION( BUILD_SHARED_LIBS "Set this to OFF to build static libraries " ON )
#OPTION( BUILD_streamlog_TESTS "Set to ON to build streamlog tests" OFF )

# project version
SET( streamlog_MAJOR_VERSION 0 )
SET( streamlog_MINOR_VERSION 1 )
SET( streamlog_PATCH_LEVEL 0 )

SET( streamlog_SOVERSION "${streamlog_MAJOR_VERSION}.${streamlog_MINOR_VERSION}" )
SET( streamlog_VERSION "${streamlog_SOVERSION}.${streamlog_PATCH_LEVEL}" )

# set default cmake build type to RelWithDebInfo
IF( NOT CMAKE_BUILD_TYPE )
	SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

# set default install prefix to project root directory
#IF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
	SET( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" )
ENDIF()

# add library install path to the rpath list
SET( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH )

# append link pathes to rpath list
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

# output directories
SET( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" CACHE PATH "EXECUTABLE_OUTPUT_PATH" FORCE )
SET( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" CACHE PATH "LIBRARY_OUTPUT_PATH" FORCE )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )

# require proper c++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# need long long for int64 for now
# FIXME: should establish wether we are on a 32bit or 64 bit machine...
#ADD_DEFINITIONS( "-Wno-long-long" )

# include directories
INCLUDE_DIRECTORIES( BEFORE ./source/include )

# get list of all source files
AUX_SOURCE_DIRECTORY( ./source/src lib_srcs )

# create library with the list of library sources
ADD_LIBRARY( lib_streamlog ${lib_srcs} )

# create symbolic lib target for calling target lib_XXX
#ADD_CUSTOM_TARGET( lib DEPENDS lib_streamlog )

# change lib_target properties
SET_TARGET_PROPERTIES( lib_streamlog PROPERTIES
	# create *nix style library versions + symbolic links
	VERSION ${streamlog_VERSION}
	SOVERSION ${streamlog_SOVERSION}
	# allow building static and shared libraries on the same build tree
	CLEAN_DIRECT_OUTPUT 1
	# avoid conflicts between library and binary target names
	OUTPUT_NAME streamlog)

# install library
INSTALL( TARGETS lib_streamlog DESTINATION lib PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE )

#-----------------------------------------------------------
ADD_SUBDIRECTORY( examples test EXCLUDE_FROM_ALL )

# create uninstall configuration file 
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
				"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
				IMMEDIATE @ONLY )

# create uninstall target
ADD_CUSTOM_TARGET( uninstall_streamlog "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" )

# create configuration file from .in file
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/streamlogConfig.cmake.in"
				"${PROJECT_BINARY_DIR}/streamlogConfig.cmake" @ONLY )

# install configuration file
INSTALL( FILES "${PROJECT_BINARY_DIR}/streamlogConfig.cmake" DESTINATION . )

# install include files
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/source/include/streamlog"
    DESTINATION include
    PATTERN "*~" EXCLUDE
    PATTERN "*CVS*" EXCLUDE )

# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
#MESSAGE( STATUS "BUILD_streamlog_TESTS = ${BUILD_streamlog_TESTS}" )
MESSAGE( STATUS "To change a value type: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

# force some variables that could be defined in the command line
# to be written to cache
SET( BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}" CACHE BOOL
	"Build shared libraries flag" FORCE )
SET( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH
	"Where to install streamlog" FORCE )
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
	"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )
SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE PATH
	"Path to custom CMake Modules" FORCE )
#SET( BUILD_streamlog_TESTS "${BUILD_streamlog_TESTS}" CACHE BOOL
#    "Set to ON to build streamlog tests" FORCE )

# export build settings
INCLUDE( CMakeExportBuildSettings )
CMAKE_EXPORT_BUILD_SETTINGS( "${PROJECT_BINARY_DIR}/streamlogBuildSettings.cmake" )
INSTALL( FILES "${PROJECT_BINARY_DIR}/streamlogBuildSettings.cmake" DESTINATION lib/cmake )

# export library dependencies (keep this as the last line in the file)
EXPORT_LIBRARY_DEPENDENCIES( "${PROJECT_BINARY_DIR}/streamlogLibDeps.cmake" )
INSTALL( FILES "${PROJECT_BINARY_DIR}/streamlogLibDeps.cmake" DESTINATION lib/cmake )

