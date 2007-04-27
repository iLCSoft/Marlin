##################################################
# cmake file for building Marlin Example Package
# @author Jan Engels, DESY
##################################################


####################################################################################################
### INCLUDES
####################################################################################################

#include directories
INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

# install include files
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include"
		DESTINATION .
		PATTERN "*~" EXCLUDE
		PATTERN "*CVS*" EXCLUDE )


####################################################################################################
### SOURCES
####################################################################################################

# require proper c++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# get list of all source files in this directory
AUX_SOURCE_DIRECTORY( . lib_srcs )


####################################################################################################
### LIBRARY
####################################################################################################

# create library
ADD_LIBRARY( lib_${PROJECT_NAME} ${lib_srcs} )

# create symbolic lib target for calling target lib_XXX
ADD_CUSTOM_TARGET( lib DEPENDS lib_${PROJECT_NAME} )

# change lib_target properties
SET_TARGET_PROPERTIES( lib_${PROJECT_NAME} PROPERTIES
	# create *nix style library versions + symbolic links
	VERSION ${${PROJECT_NAME}_VERSION}
	SOVERSION ${${PROJECT_NAME}_SOVERSION}
	# allow creating static and shared libs without conflicts
	CLEAN_DIRECT_OUTPUT 1
	# avoid conflicts between library and binary target names
	OUTPUT_NAME ${PROJECT_NAME}	)

# install library
INSTALL( TARGETS lib_${PROJECT_NAME} DESTINATION lib PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE )


####################################################################################################
### DEPENDENCIES
####################################################################################################

# load macro
IF( DEFINED ${PROJECT_NAME}_DEPENDS OR DEFINED BUILD_WITH OR DEFINED LINK_WITH )
	MESSAGE( STATUS "-------------------------------------------------------------------------------" )
	MESSAGE( STATUS "Change a module with: cmake -D<ModuleName>_HOME=<Path_to_Module>" )
	MESSAGE( STATUS )

	# load macro
	IF( NOT EXISTS "${CMAKE_MODULE_PATH}/LoadPackageMacro.cmake" )
		MESSAGE( FATAL_ERROR "\nSorry, could not find LoadPackageMacro.cmake...\n" 
			"Please set CMAKE_MODULE_PATH correctly with: "
			"cmake -DCMAKE_MODULE_PATH=<path_to_cmake_modules>" )
	ENDIF()
	INCLUDE( "${CMAKE_MODULE_PATH}/LoadPackageMacro.cmake" )
ENDIF()

# project dependencies
IF( DEFINED ${PROJECT_NAME}_DEPENDS )
	SEPARATE_ARGUMENTS( ${PROJECT_NAME}_DEPENDS )
	MARK_AS_ADVANCED( ${PROJECT_NAME}_DEPENDS )
	FOREACH( req_pkg ${${PROJECT_NAME}_DEPENDS} )
		LOAD_PACKAGE( ${req_pkg} REQUIRED )
	ENDFOREACH()
ENDIF()

# user defined dependencies
IF( DEFINED BUILD_WITH )
	SEPARATE_ARGUMENTS( BUILD_WITH )
	MARK_AS_ADVANCED( BUILD_WITH )
	FOREACH( opt_pkg ${BUILD_WITH} )
		LOAD_PACKAGE( ${opt_pkg} )
	ENDFOREACH()
	SET( BUILD_WITH "${BUILD_WITH}" CACHE STRING
		"Build ${PROJECT_NAME} with these optional packages" FORCE )
ENDIF()

# user defined dependencies
IF( DEFINED LINK_WITH )
	SEPARATE_ARGUMENTS( LINK_WITH )
	MARK_AS_ADVANCED( LINK_WITH )
	FOREACH( lnk_pkg ${LINK_WITH} )
		LOAD_PACKAGE( ${lnk_pkg} LINK_ONLY )
	ENDFOREACH()
	SET( LINK_WITH "${LINK_WITH}" CACHE STRING
		"Link ${PROJECT_NAME} with these optional packages" FORCE )
ENDIF()

IF( DEFINED ${PROJECT_NAME}_DEPENDS OR DEFINED BUILD_WITH OR DEFINED LINK_WITH )
	MESSAGE( STATUS "-------------------------------------------------------------------------------" )
ENDIF()

