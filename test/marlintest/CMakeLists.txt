########################################################
# cmake file for building Marlin example Package
# @author Jan Engels, Desy IT
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


### LIBRARY #################################################################

# library name
SET( libname marlintest )

# include directories
INCLUDE_DIRECTORIES( "include" ${PROJECT_SOURCE_DIR}/source/include  ${PROJECT_BINARY_DIR} )
#INSTALL( DIRECTORY include DESTINATION . FILES_MATCHING PATTERN "*.h" )

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )
ADD_DEFINITIONS( "-Wno-long-long" )

# add library
AUX_SOURCE_DIRECTORY( src library_sources )
ADD_SHARED_LIBRARY( ${libname} ${library_sources} )

TARGET_LINK_LIBRARIES( ${libname} Marlin )

INSTALL(TARGETS ${libname} DESTINATION lib )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

