
### LIBRARY FOR RUNNING MARLIN TESTS ########################################

# library name
SET( libname marlintest )

# include directories
INCLUDE_DIRECTORIES( BEFORE ./include ${PROJECT_SOURCE_DIR}/source/include  ${PROJECT_BINARY_DIR} )
#INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

# add library
AUX_SOURCE_DIRECTORY( ./src library_sources )
ADD_SHARED_LIBRARY( ${libname} ${library_sources} )

TARGET_LINK_LIBRARIES( ${libname} Marlin )

#INSTALL_SHARED_LIBRARY( ${libname} DESTINATION lib )
