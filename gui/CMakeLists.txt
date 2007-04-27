#####################################
# cmake file for building MarlinGUI
# @author Jan Engels, DESY
#####################################

# cmake minimum required version
CMAKE_MINIMUM_REQUIRED( VERSION 2.4 )
#SET( CMAKE_BACKWARDS_COMPATIBILITY 2.2 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

# allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )

# project name
PROJECT( MarlinGUI )

# project options
OPTION( MARLIN_USE_DLL "Set to ON to build Marlin with DLL support" OFF )

# project dependencies e.g. SET( ${PROJECT_NAME}_DEPENDS "LCIO;CLHEP" )
SET( ${PROJECT_NAME}_DEPENDS "Marlin;LCIO" )

# set default install prefix to project root directory
#IF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
	SET( CMAKE_INSTALL_PREFIX "${${PROJECT_NAME}_SOURCE_DIR}/../bin" )
ENDIF()

## append link pathes to rpath list
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

# output directories
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )

###########################################################################################################

FIND_PACKAGE( Qt4 REQUIRED )
INCLUDE(${QT_USE_FILE})

SET( cpp_srcs
	addcondition.cpp
	addprocdialog.cpp
	aprocdelegate.cpp
	dialog.cpp
	editcondition.cpp
	flowlayout.cpp
	gparamdelegate.cpp
	guihelp.cpp
	icoldelegate.cpp
	icoltdelegate.cpp
	iprocdelegate.cpp
	mainwindow.cpp
	nparamvecset.cpp
	nparamvecsetd.cpp
	ocoldelegate.cpp
	paramdelegate.cpp
)

SET( moc_hdrs
	addcondition.h
	addprocdialog.h
	aprocdelegate.h
	dialog.h
	editcondition.h
	gparamdelegate.h
	guihelp.h
	icoldelegate.h
	icoltdelegate.h
	iprocdelegate.h
	mainwindow.h
	nparamvecset.h
	nparamvecsetd.h
	ocoldelegate.h
	paramdelegate.h
)

#QT4_AUTOMOC(${cpp_srcs})
QT4_WRAP_CPP( cpp_moc_srcs ${moc_hdrs} )

# create symbolic bin target for calling targets bin_XXX
ADD_CUSTOM_TARGET( bin )

IF( MARLIN_USE_DLL )
    ADD_DEFINITIONS( "-DMARLIN_USE_DLL" )
ENDIF()

IF( MARLIN_USE_DLL )
	ADD_EXECUTABLE( ${PROJECT_NAME} main.cpp ${cpp_srcs} ${cpp_moc_srcs} ../src/ProcessorLoader )
ELSE()
	ADD_EXECUTABLE( ${PROJECT_NAME} main.cpp ${cpp_srcs} ${cpp_moc_srcs} )
ENDIF()

# tell bin target to call this bin_XXX target
ADD_DEPENDENCIES( bin ${PROJECT_NAME} )

SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES} )

INSTALL( TARGETS ${PROJECT_NAME} DESTINATION bin )


IF( DEFINED ${PROJECT_NAME}_DEPENDS OR DEFINED BUILD_WITH OR DEFINED LINK_WITH )
	#usage: LOAD_PACKAGE( Name [REQUIRED] [LINK_ONLY] )
	MACRO( LOAD_PACKAGE Name )

		IF( NOT ${Name}_CHECKED )
			SET( ${Name}_CHECKED TRUE INTERNAL "Resolve cyclic dependencies" )

			IF( NOT ${Name} STREQUAL ${PROJECT_NAME} )

				IF( NOT DEFINED ${Name}_HOME )
					MESSAGE( FATAL_ERROR "Check for ${Name}: ${Name}_HOME not set!! "
							"Please call cmake with -D${Name}_HOME=<path_to_${Name}>" )
				ENDIF()

				# create a cache entry for the Package_HOME variable
				# this is needed because command line defined variables
				# (-DVariable=Value) are not cached by default
				SET( ${Name}_HOME ${${Name}_HOME} CACHE PATH "Path to ${Name}" FORCE )

				# initialize variables
				SET( required FALSE )
				SET( link_only FALSE )

				# process arguments
				FOREACH( arg ${ARGN} )
					IF( ${arg} MATCHES "REQUIRED" )
						SET( required TRUE )
					ENDIF()
					IF( ${arg} MATCHES "LINK_ONLY" )
						SET( link_only TRUE )
					ENDIF()
				ENDFOREACH()


				# if a Config.cmake exists use it!
				IF( EXISTS ${${Name}_HOME}/${Name}Config.cmake )
					
					MESSAGE( STATUS "Check for ${Name}: ${${Name}_HOME} -- ${Name}Config.cmake found..." )
					
					# this variable needs to be set for cmake to read the config file
					SET( ${Name}_DIR ${${Name}_HOME} CACHE PATH "Path to ${Name}" )
					MARK_AS_ADVANCED( ${Name}_DIR )
					
					# as opposed to cmake Find modules the
					# Config.cmake files are not aware of this variable
					SET( ${Name}_FIND_REQUIRED ${required} )
					
					IF( required )
						FIND_PACKAGE( ${Name} REQUIRED NO_MODULE )
					ELSE()
						FIND_PACKAGE( ${Name} NO_MODULE )
					ENDIF()
				ELSE()
					MESSAGE( STATUS "Check for ${Name}: ${${Name}_HOME}" )
					IF( required )
						FIND_PACKAGE( ${Name} REQUIRED )
					ELSE()
						FIND_PACKAGE( ${Name} )
					ENDIF()
				ENDIF()

				# include directories
				IF( ${Name}_INCLUDE_DIR )
					IF( NOT link_only )
						INCLUDE_DIRECTORIES( ${${Name}_INCLUDE_DIR} )
					ENDIF()
				ENDIF()

				# set library dependencies
				IF( ${Name}_LIBRARIES )
					#LINK_LIBRARIES( ${${Name}_LIBRARIES} )
					#FIXME whole macro was pasted here due to this small difference
					TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${${Name}_LIBRARIES} )
				ENDIF()

				# add definitions
				IF( ${Name}_DEFINITIONS )
					IF( NOT link_only )
						ADD_DEFINITIONS( ${${Name}_DEFINITIONS} )
					ENDIF()
				ENDIF()
			ENDIF()
		ENDIF()

	ENDMACRO( LOAD_PACKAGE )
	
    MESSAGE( STATUS "-------------------------------------------------------------------------------" )
    MESSAGE( STATUS "Change a module with: cmake -D<ModuleName>_HOME=<Path_to_Module>" )
    MESSAGE( STATUS )
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

###########################################################################################################


## create uninstall configuration file 
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_uninstall.cmake.in"
				"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
				IMMEDIATE @ONLY )

# create uninstall target
ADD_CUSTOM_TARGET( uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" )

# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS "MARLIN_USE_DLL = ${MARLIN_USE_DLL}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

# force some variables that could be defined in the command line
# to be written to cache
SET( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH
	"Where to install ${PROJECT_NAME}" FORCE )
SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE PATH
	"Path to custom CMake Modules" FORCE )
SET( ${PROJECT_NAME}_DEPENDS "${${PROJECT_NAME}_DEPENDS}" CACHE STRING
	"${PROJECT_NAME} dependencies" FORCE )
SET( MARLIN_USE_DLL "${MARLIN_USE_DLL}" CACHE BOOL
	"Set to ON to support DLL in ${PROJECT_NAME}" FORCE )
