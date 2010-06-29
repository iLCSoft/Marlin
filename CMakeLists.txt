#################################
# cmake file for building Marlin
# @author Jan Engels, DESY
#################################

########################################################
# CMake compatibility issues: don't modify this, please!
CMAKE_MINIMUM_REQUIRED( VERSION 2.4.6 )
#SET( CMAKE_BACKWARDS_COMPATIBILITY 2.4.6 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
# allow more human readable "if then else" constructs
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
########################################################

# project name
PROJECT( Marlin )

# needed for using ctest
ENABLE_TESTING()
INCLUDE(CTest)

# allow duplicate custom targets
#SET_PROPERTY(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)
#IF(COMMAND CMAKE_POLICY)
#    CMAKE_POLICY(SET CMP0002 OLD)
#ENDIF()

# project version
SET( Marlin_MAJOR_VERSION 0 )
SET( Marlin_MINOR_VERSION 12 )
SET( Marlin_PATCH_LEVEL 0 )

# project options
OPTION( BUILD_SHARED_LIBS "Set to OFF to build static libraries" ON )
OPTION( BUILD_32BIT_COMPATIBLE "Set to ON to build in 32 bit compatibility mode" OFF )
OPTION( MARLIN_NO_DLL "Set to ON to build Marlin without DLL support" OFF )
OPTION( MARLIN_GUI "Set to ON to build Marlin GUI" OFF )
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" ON )

# project dependencies
SET( Marlin_DEPENDS "LCIO GEAR" )

# set default cmake build type to RelWithDebInfo
IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()

# set default install prefix to project root directory
#IF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
    SET( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" )
ENDIF()

#---------------- 32/64 bit issues ---------------------------------------
#INCLUDE( CheckTypeSize )
#CHECK_TYPE_SIZE( "void*" SIZE_OF_VOID_P )

IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    MESSAGE( STATUS "32 bit architecture detected" )
ENDIF()

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( STATUS "64 bit architecture detected" )

    IF( BUILD_32BIT_COMPATIBLE )
        IF( COMMAND SET_PROPERTY )
            SET_PROPERTY(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS 0)
            SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32" )
        ELSE()
            MESSAGE( FATAL_ERROR "to build in 32 bit compatibility mode you need cmake >= 2.6" )
        ENDIF()
        MESSAGE( STATUS "Build in 32 bit compatibility mode" )
    ELSE()
        MESSAGE( STATUS "Build in native 64 bit mode" )
    ENDIF()
ENDIF()
#-------------------------------------------------------------------------


# definitions
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR
    CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    ADD_DEFINITIONS( "-DMARLINDEBUG" )
ENDIF()
                                                                                                                                                           
IF( MARLIN_NO_DLL )
    ADD_DEFINITIONS( "-DMARLIN_NO_DLL" )
ENDIF()

#----- need long long for int64 for now ------
ADD_DEFINITIONS( "-Wno-long-long" )

# DOCUMENTATION
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
    INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/doc"
            DESTINATION .
            PATTERN "*CVS*" EXCLUDE
            PATTERN "*/.svn*" EXCLUDE
    )
ENDIF()

##########################################################################################

# library *nix style versioning
SET( Marlin_SOVERSION "${Marlin_MAJOR_VERSION}.${Marlin_MINOR_VERSION}" )
SET( Marlin_VERSION "${Marlin_SOVERSION}.${Marlin_PATCH_LEVEL}" )

# output directories
SET( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" CACHE PATH
    "EXECUTABLE_OUTPUT_PATH" FORCE )
SET( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" CACHE PATH
    "LIBRARY_OUTPUT_PATH" FORCE )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH )

# add install path to the rpath list
SET( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH )

# add install path to the rpath list (apple)
IF( APPLE )
    SET( CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
    MARK_AS_ADVANCED( CMAKE_INSTALL_NAME_DIR )
ENDIF()

# append link pathes to rpath list
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH 1 )
MARK_AS_ADVANCED( CMAKE_INSTALL_RPATH_USE_LINK_PATH )

# DEPENDENCIES: this code has to be placed before adding any library or
# executable so that these are linked properly against the dependencies
IF( DEFINED Marlin_DEPENDS OR DEFINED BUILD_WITH OR DEFINED LINK_WITH )
    IF( NOT DEFINED CMAKE_MODULE_PATH )
        MESSAGE( FATAL_ERROR "CMAKE_MODULE_PATH not set! Set it with: "
            "-DCMAKE_MODULE_PATH=\"/path_to_ilcsoft/CMakeModules\"" )
    ENDIF()
    # load macro
    INCLUDE( "MacroCheckDeps" )
    CHECK_DEPS()
ENDIF()

# input directories
ADD_SUBDIRECTORY( streamlog )
ADD_SUBDIRECTORY( src )

# create uninstall configuration file 
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake_uninstall.cmake.in"
                "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
                IMMEDIATE @ONLY )

# create uninstall target
ADD_CUSTOM_TARGET( uninstall "${CMAKE_COMMAND}" -P "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake" )

# create configuration file from .in file
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/MarlinConfig.cmake.in"
                "${PROJECT_BINARY_DIR}/MarlinConfig.cmake" @ONLY )

# install configuration file
INSTALL( FILES "${PROJECT_BINARY_DIR}/MarlinConfig.cmake" DESTINATION . )

# create documentation overview.html
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/doc/overview.html.in"
                "${PROJECT_BINARY_DIR}/overview.html" @ONLY )

# install configuration file
INSTALL( FILES "${PROJECT_BINARY_DIR}/overview.html" DESTINATION doc )


# display status message for important variables
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( STATUS "BUILD_32BIT_COMPATIBLE = ${BUILD_32BIT_COMPATIBLE}" )
ENDIF()
MESSAGE( STATUS "MARLIN_NO_DLL = ${MARLIN_NO_DLL}" )
MESSAGE( STATUS "MARLIN_GUI = ${MARLIN_GUI}" )
MESSAGE( STATUS "BUILD_WITH = \"${BUILD_WITH}\"" )
MESSAGE( STATUS "INSTALL_DOC = ${INSTALL_DOC}" )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

# force some variables that could be defined in the command line
# to be written to cache
SET( BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}" CACHE BOOL
    "Set to OFF to build static libraries" FORCE )
SET( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH
    "Where to install Marlin" FORCE )
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )
SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE PATH
    "Path to custom CMake Modules" FORCE )
SET( MARLIN_NO_DLL "${MARLIN_NO_DLL}" CACHE BOOL
    "Set to ON to build Marlin without DLL support" FORCE )
SET( MARLIN_GUI "${MARLIN_GUI}" CACHE BOOL
    "Set to ON to build Marlin GUI" FORCE )
SET( INSTALL_DOC "${INSTALL_DOC}" CACHE BOOL
    "Set to OFF to skip build/install Documentation" FORCE )

# export build settings
INCLUDE( CMakeExportBuildSettings )
CMAKE_EXPORT_BUILD_SETTINGS( "MarlinBuildSettings.cmake" )
INSTALL( FILES "${PROJECT_BINARY_DIR}/MarlinBuildSettings.cmake" DESTINATION lib/cmake )

# export library dependencies (keep this as the last line in the file)
EXPORT_LIBRARY_DEPENDENCIES( "MarlinLibDeps.cmake" )
INSTALL( FILES "${PROJECT_BINARY_DIR}/MarlinLibDeps.cmake" DESTINATION lib/cmake )
