#################################
# cmake file for building Marlin
# @author Jan Engels, DESY
#################################

# include directories
INCLUDE_DIRECTORIES( BEFORE "./include" )
INCLUDE_DIRECTORIES( BEFORE "${PROJECT_BINARY_DIR}" ) # generated MarlinConfig.h


# install header files
FILE( GLOB marlin_headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" ./include/marlin/*.h ./tinyxml/include/marlin/*.h )

IF( NOT MARLIN_AIDA )
    LIST( REMOVE_ITEM marlin_headers ./include/marlin/AIDAProcessor.h )
ENDIF()

IF( NOT MARLIN_LCCD )
    LIST( REMOVE_ITEM marlin_headers ./include/marlin/ConditionsProcessor.h )
ENDIF()

IF( NOT MARLIN_CLHEP )
    LIST( REMOVE_ITEM marlin_headers
        ./include/marlin/IFourVectorSmearer.h
        ./include/marlin/SimpleClusterSmearer.h
        ./include/marlin/SimpleParticleFactory.h
        ./include/marlin/SimpleTrackSmearer.h
    )
ENDIF()

INSTALL( FILES ${marlin_headers} DESTINATION include/marlin )



# require proper c++
#ADD_DEFINITIONS( "-Wall -ansi -pedantic" )
#ADD_DEFINITIONS( "-Wno-long-long" )

# definitions
# FIXME rather add to CMAKE_CXX_Debug_FLAGS ...
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR
    CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    ADD_DEFINITIONS( "-DMARLINDEBUG" )
ENDIF()


AUX_SOURCE_DIRECTORY( ./src library_sources )
AUX_SOURCE_DIRECTORY( ./tinyxml/src tinyxml_sources )
INCLUDE_DIRECTORIES( SYSTEM tinyxml/include )

# need to remove ProcessorLoader.cc and Marlin.cc from library sources
LIST( REMOVE_ITEM library_sources ./src/Marlin.cc ./src/ProcessorLoader.cc )

# create library
ADD_SHARED_LIBRARY( Marlin ${library_sources} )

ADD_SHARED_LIBRARY( MarlinXML ${tinyxml_sources} )
SET_TARGET_PROPERTIES( MarlinXML  PROPERTIES COMPILE_FLAGS "-w" )
INSTALL_SHARED_LIBRARY( MarlinXML DESTINATION ${CMAKE_INSTALL_LIBDIR} )

INSTALL_SHARED_LIBRARY( Marlin DESTINATION ${CMAKE_INSTALL_LIBDIR} )
TARGET_LINK_LIBRARIES( Marlin ${CMAKE_DL_LIBS} MarlinXML ${Marlin_DEPENDS_LIBRARIES}
  ## Note: it is not a problem if any variables is empty because package not found
  ${AIDA_LIBRARIES} ${CLHEP_LIBRARIES} ${LCCD_LIBRARIES} )



# ----- Marlin executable ----------------------------------------------------
ADD_EXECUTABLE( bin_Marlin ./src/Marlin.cc ./src/ProcessorLoader.cc )
# avoid name conflict between Marlin library and Marlin application
SET_TARGET_PROPERTIES( bin_Marlin PROPERTIES OUTPUT_NAME Marlin )
TARGET_LINK_LIBRARIES( bin_Marlin Marlin )
INSTALL( TARGETS bin_Marlin DESTINATION bin )
# ----------------------------------------------------------------------------



# ----- MarlinGUI ------------------------------------------------------------
IF( MARLIN_GUI )
    ADD_SUBDIRECTORY( ./gui )
ENDIF()
# ----------------------------------------------------------------------------

