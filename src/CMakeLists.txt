#################################
# cmake file for building Marlin
# @author Jan Engels, DESY
#################################

####################################################################################################
### INCLUDES
####################################################################################################

# include directories
INCLUDE_DIRECTORIES( "${${PROJECT_NAME}_SOURCE_DIR}/include" )

# install include files
INSTALL( DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/include/marlin" DESTINATION include
        PATTERN "*~" EXCLUDE
        PATTERN "*CVS*" EXCLUDE )


####################################################################################################
### SOURCES
####################################################################################################

# require proper c++
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )

# add debug definitions
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR
    CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    ADD_DEFINITIONS( "-DMARLINDEBUG" )
ENDIF()

IF( MARLIN_NO_DLL )
    ADD_DEFINITIONS( "-DMARLIN_NO_DLL" )
ENDIF()

# binary sources: list of all sources from which you want to create a binary
SET( bin_srcs Marlin.cc )

# sources needed for binaries that do not belong to the library
SET( ext_bin_srcs ProcessorLoader.cc )

# get list of all source files in this directory
AUX_SOURCE_DIRECTORY( . all_srcs )

# remove "./" from the list of source files
STRING( REPLACE "./" "" all_srcs "${all_srcs}" )

# exclude sources: filter out the sources specified above from the list of all sources
IF( bin_srcs OR ext_bin_srcs )
    LIST( REMOVE_ITEM all_srcs ${bin_srcs} ${ext_bin_srcs} )
ENDIF()

# library sources
SET( lib_srcs ${all_srcs} )

# gui stuff
IF( MARLIN_GUI )

    SET( ENV{PATH} "${QT4_HOME}/bin:$ENV{PATH}" )
    FIND_PACKAGE( Qt4 REQUIRED )
    INCLUDE(${QT_USE_FILE})

    SET( cpp_srcs
        ../gui/addcondition.cpp
        ../gui/addprocdialog.cpp
        ../gui/aprocdelegate.cpp
        ../gui/dialog.cpp
        ../gui/editcondition.cpp
        ../gui/flowlayout.cpp
        ../gui/gparamdelegate.cpp
        ../gui/guihelp.cpp
        ../gui/icoldelegate.cpp
        ../gui/icoltdelegate.cpp
        ../gui/iprocdelegate.cpp
        ../gui/mainwindow.cpp
        ../gui/nparamvecset.cpp
        ../gui/nparamvecsetd.cpp
        ../gui/ocoldelegate.cpp
        ../gui/paramdelegate.cpp
    )
    SET( moc_hdrs
        ../gui/addcondition.h
        ../gui/addprocdialog.h
        ../gui/aprocdelegate.h
        ../gui/dialog.h
        ../gui/editcondition.h
        ../gui/gparamdelegate.h
        ../gui/guihelp.h
        ../gui/icoldelegate.h
        ../gui/icoltdelegate.h
        ../gui/iprocdelegate.h
        ../gui/mainwindow.h
        ../gui/nparamvecset.h
        ../gui/nparamvecsetd.h
        ../gui/ocoldelegate.h
        ../gui/paramdelegate.h
    )

    # generate moc files
    QT4_WRAP_CPP( cpp_moc_srcs ${moc_hdrs} )
    
ENDIF()


####################################################################################################
### LIBRARY
####################################################################################################

# create library
ADD_LIBRARY( lib_${PROJECT_NAME} ${lib_srcs} )

# create symbolic lib target for calling target lib_XXX
ADD_CUSTOM_TARGET( lib DEPENDS lib_${PROJECT_NAME} )

# change lib_target properties
SET_TARGET_PROPERTIES( lib_${PROJECT_NAME} PROPERTIES
    # create *nix style library versions + symbolic links
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME ${PROJECT_NAME} )

# install library
INSTALL( TARGETS lib_${PROJECT_NAME} DESTINATION lib PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE )

####################################################################################################
### BINS
####################################################################################################

IF( bin_srcs )

    # check if we are on a *nix system
    IF( NOT UNIX OR APPLE )
        IF( DEFINED LINK_STATIC_WHOLE_LIBS )
            MESSAGE( FATAL_ERROR "The LINK_STATIC_WHOLE_LIBS option is only avaliabe for *nix systems" )
        ENDIF()
        IF( NOT BUILD_SHARED_LIBS )
            MESSAGE( FATAL_ERROR "You can only build Marlin statically under *nix systems" )
        ENDIF()
    ENDIF()
    
    # create symbolic bin target for calling targets bin_XXX
    ADD_CUSTOM_TARGET( bin )

    # create bin targets (remove .cc extensions from binary sources)
    FOREACH( src ${bin_srcs} )
        STRING( REPLACE ".cc" "" bin ${src} )
        LIST( APPEND bins ${bin} )
    ENDFOREACH()

    IF( NOT MARLIN_NO_DLL )
        LINK_LIBRARIES( ${CMAKE_DL_LIBS} )
    ENDIF()

    # whole archive libraries
    IF( DEFINED LINK_STATIC_WHOLE_LIBS )
        SEPARATE_ARGUMENTS( LINK_STATIC_WHOLE_LIBS )
        SET( LINK_STATIC_WHOLE_LIBS "${LINK_STATIC_WHOLE_LIBS}" CACHE STRING
                "Link ${PROJECT_NAME} with these static libs" FORCE )
        MESSAGE( STATUS "Following static libraries are going to be linked to the binaries with the" )
        MESSAGE( STATUS "-Wl,--whole-archive linker flag:" )
        FOREACH( lib ${LINK_STATIC_WHOLE_LIBS} )
             MESSAGE( STATUS "${lib}" )
        ENDFOREACH()
    ENDIF()


    # loop through list of binaries
    FOREACH( bin ${bins} )
        
        # create target for binary
        ADD_EXECUTABLE( bin_${bin} ${bin}.cc ${ext_bin_srcs} )
        
        # tell bin target to call this bin_XXX target
        ADD_DEPENDENCIES( bin bin_${bin} )
        
        # avoid conflicts between library and binary target names
        SET_TARGET_PROPERTIES( bin_${bin} PROPERTIES OUTPUT_NAME ${bin} )
        
        # link against project library
        IF( BUILD_SHARED_LIBS )
            TARGET_LINK_LIBRARIES( bin_${bin} lib_${PROJECT_NAME} )
        ELSE()
            TARGET_LINK_LIBRARIES( bin_${bin} -Wl,--whole-archive lib_${PROJECT_NAME} -Wl,--no-whole-archive )
        ENDIF()
        
        # whole archive libraries
        IF( DEFINED LINK_STATIC_WHOLE_LIBS )
            FOREACH( lib ${LINK_STATIC_WHOLE_LIBS} )
                TARGET_LINK_LIBRARIES( bin_${bin} -Wl,--whole-archive ${lib} -Wl,--no-whole-archive )
            ENDFOREACH()
        ENDIF()
        
        # install binary commando
        INSTALL( TARGETS bin_${bin} DESTINATION bin )
    ENDFOREACH()

    IF( MARLIN_GUI )
        
        # create target for binary
        ADD_EXECUTABLE( bin_${PROJECT_NAME}_GUI ../gui/main.cpp ${cpp_srcs} ${cpp_moc_srcs} ${ext_bin_srcs} )
        
        # tell bin target to call this bin_XXX target
        ADD_DEPENDENCIES( bin bin_${PROJECT_NAME}_GUI )
        
        # avoid conflicts between library and binary target names
        SET_TARGET_PROPERTIES( bin_${PROJECT_NAME}_GUI PROPERTIES OUTPUT_NAME ${PROJECT_NAME}GUI )
        
        # link against project library
        IF( BUILD_SHARED_LIBS )
            TARGET_LINK_LIBRARIES( bin_${PROJECT_NAME}_GUI lib_${PROJECT_NAME} ${QT_LIBRARIES} )
        ELSE()
            TARGET_LINK_LIBRARIES( bin_${PROJECT_NAME}_GUI -Wl,--whole-archive lib_${PROJECT_NAME} -Wl,--no-whole-archive ${QT_LIBRARIES} )
        ENDIF()

        # whole archive libraries
        IF( DEFINED LINK_STATIC_WHOLE_LIBS )
            FOREACH( lib ${LINK_STATIC_WHOLE_LIBS} )
                TARGET_LINK_LIBRARIES( bin_${PROJECT_NAME}_GUI -Wl,--whole-archive ${lib} -Wl,--no-whole-archive )
            ENDFOREACH()
        ENDIF()

        # install binary commando
        INSTALL( TARGETS bin_${PROJECT_NAME}_GUI DESTINATION bin )
    ENDIF()

ENDIF()

