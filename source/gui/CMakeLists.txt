######################################
# cmake file for building MarlinGUI
# @author Jan Engels, DESY
######################################

# cmake 2.6 throws warning because QT_LIBRARIES contains -lpthread -ldl
# without full path
#IF(COMMAND CMAKE_POLICY)
#    CMAKE_POLICY(SET CMP0003 NEW)
#ENDIF()

INCLUDE(${QT_USE_FILE})

AUX_SOURCE_DIRECTORY( . gui_sources )

IF( APPLE)
    #fg:  needed when the QT framework was installed via dmg....
    #ADD_DEFINITIONS( "-DQT_EDITION=0x0003" )
    SET_SOURCE_FILES_PROPERTIES( ${gui_sources}
        PROPERTIES COMPILE_FLAGS "-DQT_EDITION=0x0003"
    )
ENDIF( APPLE )

# generate moc files
FILE( GLOB gui_headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h )
LIST( REMOVE_ITEM gui_headers flowlayout.h )
QT4_WRAP_CPP( gui_moc_sources ${gui_headers} )

# MarlinGUI executable
ADD_EXECUTABLE( MarlinGUI ${gui_sources} ${gui_moc_sources} ../src/ProcessorLoader.cc )
TARGET_LINK_LIBRARIES( MarlinGUI Marlin ${QT_LIBRARIES} )
INSTALL( TARGETS MarlinGUI DESTINATION bin )

