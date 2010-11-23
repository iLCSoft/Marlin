#############################################################################
# cmake example build setup for Marlin
# edit accordingly - pathes to packages !
# 
#
# For building Marlin with cmake type:
# (1) $ mkdir build
# (2) $ cd build
# (3) $ cmake -C ../BuildSetup.cmake ..
#         or to use an existing ilcsoft installation
#       cmake -C ../BuildSetup.cmake -C $ILCSOFT/vXX-YY/ILCSoft.cmake ..
# (4) $ make install
#
# @author Jan Engels, DESY
#############################################################################



#############################################################################
# REQUIRED PACKAGES
#############################################################################

# ILC_HOME
SET( ILC_HOME "/path/to/ilcsoft" CACHE PATH "Path to ILC Software" FORCE )

# Path to LCIO
SET( LCIO_HOME "${ILC_HOME}/lcio/vXX-YY" CACHE PATH "Path to LCIO" FORCE )

# Path to GEAR
SET( GEAR_HOME "${ILC_HOME}/gear/vXX-YY" CACHE PATH "Path to GEAR" FORCE )



#############################################################################
# Marlin GUI
#############################################################################

# You need to set QT4_HOME path if you want to build the GUI
#SET( MARLIN_GUI OFF CACHE BOOL "Set to ON to build Marlin GUI" FORCE )

# Path to QT4 (needed for MARLIN_GUI)
#SET( QT4_HOME "/path/to/QT4" CACHE PATH "Path to QT4" FORCE )
 


#############################################################################
# Optional packages
#############################################################################

# Path to LCCD
SET( LCCD_HOME "${ILC_HOME}/lccd/vXX-YY" CACHE PATH "Path to LCCD" FORCE )

# Path to CLHEP
SET( CLHEP_HOME "${ILC_HOME}/CLHEP/vXX-YY" CACHE PATH "Path to CLHEP" FORCE )

# Path to RAIDA
#SET( RAIDA_HOME "${ILC_HOME}/RAIDA/vXX-YY" CACHE PATH "Path to RAIDA" FORCE )

# CMake Modules Path
#SET( CMAKE_MODULE_PATH "${ILC_HOME}/CMakeModules/vXX-YY" CACHE PATH "Path to CMake Modules" FORCE )



#############################################################################
# Project options
#############################################################################

#SET( INSTALL_DOC OFF CACHE BOOL "Set to OFF to skip build/install Documentation" FORCE )

# set cmake build type, default value is: RelWithDebInfo
# possible options are: None Debug Release RelWithDebInfo MinSizeRel
#SET( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE )



#############################################################################
# Advanced options
#############################################################################

# installation path for Marlin
#SET( CMAKE_INSTALL_PREFIX "/foo/bar" CACHE STRING "Where to install Marlin" FORCE )

