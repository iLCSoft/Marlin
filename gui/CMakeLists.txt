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
# FIXME not supported on this makefile yet
#OPTION( MARLIN_USE_DLL "Set to ON to build Marlin with DLL support" OFF )

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

# FIXME has to be calles lib_XXX due to LOAD_PACKAGE Macro
ADD_EXECUTABLE( lib_${PROJECT_NAME} main.cpp ${cpp_srcs} ${cpp_moc_srcs}
../src/ProcessorLoader )

# tell bin target to call this bin_XXX target
ADD_DEPENDENCIES( bin lib_${PROJECT_NAME} )

SET_TARGET_PROPERTIES( lib_${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME} )

TARGET_LINK_LIBRARIES( lib_${PROJECT_NAME} ${QT_LIBRARIES} )

INSTALL( TARGETS lib_${PROJECT_NAME} DESTINATION bin )

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
#MESSAGE( STATUS "MARLIN_USE_DLL = ${MARLIN_USE_DLL}" )
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
#SET( MARLIN_USE_DLL "${MARLIN_USE_DLL}" CACHE BOOL
#	"Set to ON to support DLL in ${PROJECT_NAME}" FORCE )
