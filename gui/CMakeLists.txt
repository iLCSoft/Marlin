#####################################
# cmake file for building MarlinGUI
# @author Jan Engels, DESY
#####################################

########################################################
# CMake compatibility issues: don't modify this, please!
CMAKE_MINIMUM_REQUIRED( VERSION 2.4.6 )
#SET( CMAKE_BACKWARDS_COMPATIBILITY 2.4.6 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
# allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
########################################################

# project name
PROJECT( MarlinGUI )

# project options
OPTION( MARLIN_NO_DLL "Set to ON to build MarlinGUI without DLL support" OFF )

# project dependencies e.g. SET( ${PROJECT_NAME}_DEPENDS "LCIO;CLHEP" )
SET( ${PROJECT_NAME}_DEPENDS "Marlin;LCIO" )

# set default install prefix to project root directory
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
    SET( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/.." )
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

IF( MARLIN_NO_DLL )
    ADD_DEFINITIONS( "-DMARLIN_NO_DLL" )
ENDIF()

####################################################################################################
### DEPENDENCIES
####################################################################################################

# load macro
IF( DEFINED ${PROJECT_NAME}_DEPENDS OR DEFINED BUILD_WITH OR DEFINED LINK_WITH )
    # load macro
    IF( NOT EXISTS "${CMAKE_MODULE_PATH}/MacroCheckDeps.cmake" )
        MESSAGE( FATAL_ERROR "\nSorry, could not find MacroCheckDeps.cmake...\n"
            "Please set CMAKE_MODULE_PATH correctly with: "
            "cmake -DCMAKE_MODULE_PATH=<path_to_cmake_modules>" )
    ENDIF()
    INCLUDE( "${CMAKE_MODULE_PATH}/MacroCheckDeps.cmake" )
    CHECK_DEPS()
ENDIF()

####################################################################################################

# create symbolic bin target for calling targets bin_XXX
ADD_CUSTOM_TARGET( bin )

ADD_EXECUTABLE( ${PROJECT_NAME} main.cpp ${cpp_srcs} ${cpp_moc_srcs} ../src/ProcessorLoader )

# tell bin target to call this bin_XXX target
ADD_DEPENDENCIES( bin ${PROJECT_NAME} )

# whole archive libraries
IF( DEFINED LINK_STATIC_WHOLE_LIBS )
    IF( UNIX )
        SEPARATE_ARGUMENTS( LINK_STATIC_WHOLE_LIBS )
        SET( LINK_STATIC_WHOLE_LIBS "${LINK_STATIC_WHOLE_LIBS}" CACHE STRING
                "Link ${PROJECT_NAME} with these static libs" FORCE )
        MESSAGE( STATUS "Following static libraries are going to be linked to ${PROJECT_NAME} with the" )
        MESSAGE( STATUS "-Wl,--whole-archive linker flag:" )
        FOREACH( lib ${LINK_STATIC_WHOLE_LIBS} )
            MESSAGE( STATUS "${lib}" )
            TARGET_LINK_LIBRARIES( ${PROJECT_NAME} -Wl,--whole-archive ${lib} -Wl,--no-whole-archive )
        ENDFOREACH()
    ELSE()
        MESSAGE( FATAL_ERROR "The LINK_STATIC_WHOLE_LIBS option is only avaliabe for *nix systems" )
    ENDIF()
ENDIF()

# link against qt libraries
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES} )

# install commando
INSTALL( TARGETS ${PROJECT_NAME} DESTINATION bin )

###########################################################################################################

## create uninstall configuration file 
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/../cmake_uninstall.cmake.in"
                "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
                IMMEDIATE @ONLY )

# create uninstall target
ADD_CUSTOM_TARGET( uninstall "${CMAKE_COMMAND}" -P "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake" )

# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS "MARLIN_NO_DLL = ${MARLIN_NO_DLL}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

# force some variables that could be defined in the command line
# to be written to cache
SET( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH
    "Where to install ${PROJECT_NAME}" FORCE )
SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE PATH
    "Path to custom CMake Modules" FORCE )
SET( MARLIN_NO_DLL "${MARLIN_NO_DLL}" CACHE BOOL
    "Set to ON to build MarlinGUI without DLL support" FORCE )
