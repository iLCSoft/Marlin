#################################
# cmake file for building Marlin
# @author Jan Engels, DESY
#################################

# include directories
INCLUDE_DIRECTORIES( BEFORE "${Marlin_BINARY_DIR}" ) # generated MarlinConfig.h
INCLUDE_DIRECTORIES( BEFORE "./include" )
INCLUDE_DIRECTORIES( BEFORE "${Marlin_SOURCE_DIR}/streamlog/source/include" )


# install header files
FILE( GLOB marlin_headers ./include/marlin/*.h )

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
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )
ADD_DEFINITIONS( "-Wno-long-long" )

# definitions
# FIXME rather add to CMAKE_CXX_Debug_FLAGS ...
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR
    CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    ADD_DEFINITIONS( "-DMARLINDEBUG" )
ENDIF()

AUX_SOURCE_DIRECTORY( ./src library_sources )

# need to remove ProcessorLoader.cc and Marlin.cc from library sources
LIST( REMOVE_ITEM library_sources ./src/Marlin.cc ./src/ProcessorLoader.cc )

# create library
ADD_SHARED_LIBRARY( Marlin ${library_sources} )
INSTALL_SHARED_LIBRARY( Marlin DESTINATION lib )
TARGET_LINK_LIBRARIES( Marlin streamlog ${CMAKE_DL_LIBS} )



# ----- Marlin executable ----------------------------------------------------
ADD_EXECUTABLE( bin_Marlin ./src/Marlin.cc ./src/ProcessorLoader.cc )
# avoid name conflict between Marlin library and Marlin application
SET_TARGET_PROPERTIES( bin_Marlin PROPERTIES OUTPUT_NAME Marlin )
TARGET_LINK_LIBRARIES( bin_Marlin Marlin )
INSTALL( TARGETS bin_Marlin DESTINATION bin )
# ----------------------------------------------------------------------------



# ----- MarlinGUI ------------------------------------------------------------
IF( MARLIN_GUI )

    # cmake 2.6 throws warning because QT_LIBRARIES contains -lpthread -ldl
    # without full path
    IF(COMMAND CMAKE_POLICY)
        CMAKE_POLICY(SET CMP0003 NEW)
    ENDIF()

    # FindQt4 looks for qmake in $PATH
    SET( ENV{PATH} "${QT4_HOME}/bin:$ENV{PATH}" )
    FIND_PACKAGE( Qt4 REQUIRED )
    INCLUDE(${QT_USE_FILE})

    IF( APPLE)
       #fg:  needed when the QT framework was installed via dmg....
      ADD_DEFINITIONS( "-DQT_EDITION=0x0003" )
    ENDIF( APPLE )

    AUX_SOURCE_DIRECTORY( ./gui gui_sources )
    #FILE( GLOB gui_sources ./gui/*.cpp )
    #LIST( REMOVE_ITEM gui_sources ./gui/main.cpp )

    FILE( GLOB gui_headers ./gui/*.h )
    LIST( REMOVE_ITEM gui_headers ./gui/flowlayout.h )

    # generate moc files
    QT4_WRAP_CPP( gui_moc_sources ${gui_headers} )
    
    ADD_EXECUTABLE( MarlinGUI ${gui_sources} ${gui_moc_sources} ./src/ProcessorLoader.cc )
    TARGET_LINK_LIBRARIES( MarlinGUI Marlin ${QT_LIBRARIES} )
    INSTALL( TARGETS MarlinGUI DESTINATION bin )

ENDIF()
# ----------------------------------------------------------------------------

