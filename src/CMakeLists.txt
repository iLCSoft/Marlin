#################################
# cmake file for building Marlin
# @author Jan Engels, DESY
#################################

####################################################################################################
### INCLUDES
####################################################################################################

# include directories
INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

# install include files
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/marlin" DESTINATION include
		PATTERN "*~" EXCLUDE
		PATTERN "*CVS*" EXCLUDE )


####################################################################################################
### SOURCES
####################################################################################################

# require proper c++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

IF( MARLIN_USE_DLL )
	ADD_DEFINITIONS( "-DMARLIN_USE_DLL" )
ENDIF()

# binary sources: list of all sources from which you want to create a binary
SET( bin_srcs Marlin.cc )

# sources needed for binaries that do not belong to the library
SET( ext_bin_srcs ProcessorLoader.cc )

# get list of all source files in this directory
AUX_SOURCE_DIRECTORY( . all_srcs )

# remove "./" from the list of source files
STRING( REPLACE "./" "" all_srcs "${all_srcs}" )

# exclude sources: filter out the sources specified above from the list of all sources
IF( bin_srcs OR ext_bin_srcs )
	LIST( REMOVE_ITEM all_srcs ${bin_srcs} ${ext_bin_srcs} )
ENDIF()

# library sources
SET( lib_srcs ${all_srcs} )


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
	OUTPUT_NAME ${PROJECT_NAME} )

# install library
INSTALL( TARGETS lib_${PROJECT_NAME} DESTINATION lib PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE )


####################################################################################################
### BINS
####################################################################################################

IF( bin_srcs )
	# create symbolic bin target for calling targets bin_XXX
	ADD_CUSTOM_TARGET( bin )

	# create bin targets (remove .cc extensions from binary sources)
	FOREACH( src ${bin_srcs} )
		STRING( REPLACE ".cc" "" bin ${src} )
		LIST( APPEND bins ${bin} )
	ENDFOREACH()

	# loop through list of binaries
	FOREACH( bin ${bins} )
		# create target for binary
		IF( MARLIN_USE_DLL )
			ADD_EXECUTABLE( bin_${bin} EXCLUDE_FROM_ALL ${bin}.cc ${ext_bin_srcs} )
		ELSE()
			ADD_EXECUTABLE( bin_${bin} EXCLUDE_FROM_ALL ${bin}.cc )
		ENDIF()
		# tell bin target to call this bin_XXX target
		ADD_DEPENDENCIES( bin bin_${bin} )
		# avoid conflicts between library and binary target names
		SET_TARGET_PROPERTIES( bin_${bin} PROPERTIES OUTPUT_NAME ${bin} )
		IF( MARLIN_USE_DLL )
			# link against project library and DLL lib
			TARGET_LINK_LIBRARIES( bin_${bin} lib_${PROJECT_NAME} ${CMAKE_DL_LIBS} )
		ELSE()
			# link against project library
			TARGET_LINK_LIBRARIES( bin_${bin} lib_${PROJECT_NAME} )
		ENDIF()
		#IF( BUILD_SHARED_LIBS )
		#	TARGET_LINK_LIBRARIES( bin_${bin} lib_${PROJECT_NAME} ${CMAKE_DL_LIBS} )
		#ELSE()
		#	TARGET_LINK_LIBRARIES( bin_${bin} -Wl,--whole-archive lib_${PROJECT_NAME} -Wl,--no-whole-archive ${CMAKE_DL_LIBS} )
		#	# whole archive libraries
		#	FOREACH( lib ${WHOLE_ARCHIVE} )
		#		TARGET_LINK_LIBRARIES( bin_${bin} -Wl,--whole-archive ${lib} -Wl,--no-whole-archive )
		#	ENDFOREACH()
		#ENDIF()
		# install binary commando
		INSTALL( TARGETS bin_${bin} DESTINATION bin )
	ENDFOREACH()
ENDIF()


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
		LOAD_PACKAGE( ${opt_pkg} REQUIRED )
	ENDFOREACH()
	SET( BUILD_WITH "${BUILD_WITH}" CACHE STRING
		"Build ${PROJECT_NAME} with these optional packages" FORCE )
ENDIF()

# user defined dependencies
IF( DEFINED LINK_WITH )
	SEPARATE_ARGUMENTS( LINK_WITH )
	MARK_AS_ADVANCED( LINK_WITH )
	FOREACH( lnk_pkg ${LINK_WITH} )
		LOAD_PACKAGE( ${lnk_pkg} REQUIRED LINK_ONLY )
	ENDFOREACH()
	SET( LINK_WITH "${LINK_WITH}" CACHE STRING
		"Link ${PROJECT_NAME} with these optional packages" FORCE )
ENDIF()

IF( DEFINED ${PROJECT_NAME}_DEPENDS OR DEFINED BUILD_WITH OR DEFINED LINK_WITH )
	MESSAGE( STATUS "-------------------------------------------------------------------------------" )
ENDIF()

