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

IF( MARLIN_USE_DLL )
	ADD_DEFINITIONS( "-DMARLIN_USE_DLL" )
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
	# create symbolic bin target for calling targets bin_XXX
	ADD_CUSTOM_TARGET( bin )

	# create bin targets (remove .cc extensions from binary sources)
	FOREACH( src ${bin_srcs} )
		STRING( REPLACE ".cc" "" bin ${src} )
		LIST( APPEND bins ${bin} )
	ENDFOREACH()

	IF( MARLIN_USE_DLL )
		LINK_LIBRARIES( ${CMAKE_DL_LIBS} )
	ENDIF()

	# loop through list of binaries
	FOREACH( bin ${bins} )
		# create target for binary
		IF( MARLIN_USE_DLL )
			ADD_EXECUTABLE( bin_${bin} ${bin}.cc ${ext_bin_srcs} )
		ELSE()
			ADD_EXECUTABLE( bin_${bin} ${bin}.cc )
		ENDIF()
		# tell bin target to call this bin_XXX target
		ADD_DEPENDENCIES( bin bin_${bin} )
		# avoid conflicts between library and binary target names
		SET_TARGET_PROPERTIES( bin_${bin} PROPERTIES OUTPUT_NAME ${bin} )
		# link against project library
		TARGET_LINK_LIBRARIES( bin_${bin} lib_${PROJECT_NAME} )
		#IF( BUILD_SHARED_LIBS )
		#	TARGET_LINK_LIBRARIES( bin_${bin} lib_${PROJECT_NAME} )
		#ELSE()
		#	TARGET_LINK_LIBRARIES( bin_${bin} -Wl,--whole-archive lib_${PROJECT_NAME} -Wl,--no-whole-archive )
		#	# whole archive libraries
		#	FOREACH( lib ${WHOLE_ARCHIVE} )
		#		TARGET_LINK_LIBRARIES( bin_${bin} -Wl,--whole-archive ${lib} -Wl,--no-whole-archive )
		#	ENDFOREACH()
		#ENDIF()
		# install binary commando
		INSTALL( TARGETS bin_${bin} DESTINATION bin )
	ENDFOREACH()

	IF( MARLIN_GUI )
		# create target for binary
		IF( MARLIN_USE_DLL )
			ADD_EXECUTABLE( bin_${PROJECT_NAME}_GUI ../gui/main.cpp ${cpp_srcs} ${cpp_moc_srcs} ${ext_bin_srcs} )
		ELSE()
			ADD_EXECUTABLE( bin_${PROJECT_NAME}_GUI ../gui/main.cpp ${cpp_srcs} ${cpp_moc_srcs} )
		ENDIF()
		# tell bin target to call this bin_XXX target
		ADD_DEPENDENCIES( bin bin_${PROJECT_NAME}_GUI )
		# avoid conflicts between library and binary target names
		SET_TARGET_PROPERTIES( bin_${PROJECT_NAME}_GUI PROPERTIES OUTPUT_NAME ${PROJECT_NAME}GUI )
		# link against project library
		TARGET_LINK_LIBRARIES( bin_${PROJECT_NAME}_GUI lib_${PROJECT_NAME} ${QT_LIBRARIES} )
		# install binary commando
		INSTALL( TARGETS bin_${PROJECT_NAME}_GUI DESTINATION bin )
	ENDIF()

ENDIF()



