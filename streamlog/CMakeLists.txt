########################################################
# cmake file for building Marlin example Package
# @author Jan Engels, Desy IT
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( streamlog )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )



### DEPENDENCIES ############################################################

# FindILCSOFT_CMAKE_MODULES.cmake needs to be in CMAKE_MODULE_PATH
FIND_PACKAGE( ILCSOFT_CMAKE_MODULES 0.11 REQUIRED )

# load default settings from ILCSOFT_CMAKE_MODULES
#CMAKE_POLICY(SET CMP0002 OLD) # avoid error from duplicate uninstall target (Marlin vs streamlog)
#INCLUDE( ilcsoft_default_settings )

# FIXME: cannot INCLUDE( ilcsoft_default_settings ) due to duplicate uninstall target
# even if setting POLICY CMP0002 to OLD 'make uninstall' returns error
INCLUDE( ilcsoft_default_install_prefix )
INCLUDE( ilcsoft_default_build_type )
INCLUDE( ilcsoft_default_enable_ctest )
INCLUDE( ilcsoft_default_library_versioning )
INCLUDE( ilcsoft_default_build_output_directories )
INCLUDE( ilcsoft_default_rpath_settings )
#INCLUDE( ilcsoft_default_uninstall_target )



### LIBRARY #################################################################

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# include directories
INCLUDE_DIRECTORIES( BEFORE ./source/include )
INSTALL_DIRECTORY( ./source/include DESTINATION . FILES_MATCHING PATTERN "*.h" )

# add library
AUX_SOURCE_DIRECTORY( ./source/src library_sources )
ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${library_sources} )
INSTALL_SHARED_LIBRARY( ${PROJECT_NAME} DESTINATION lib )


ADD_SUBDIRECTORY( ./examples )


# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

