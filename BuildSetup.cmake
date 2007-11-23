#############################################################################
# cmake build setup for Marlin
#
# For building Marlin with cmake type:
# (1) $ mkdir build
# (2) $ cd build
# (3) $ cmake -C ../BuildSetup.cmake ..
# (4) $ make install
#
# @author Jan Engels, DESY
#############################################################################


#############################################################################
# Setup path variables
#############################################################################

# ILC_HOME
SET( ILC_HOME "/afs/desy.de/group/it/ilcsoft/v01-03"
    CACHE PATH "Path to ILC Software" FORCE )

# Path to LCIO
SET( LCIO_HOME "${ILC_HOME}/lcio/v01-09"
    CACHE PATH "Path to LCIO" FORCE )

# CMake Modules Path
SET( CMAKE_MODULE_PATH "${ILC_HOME}/CMakeModules/v01-04-01"
    CACHE PATH "Path to CMake Modules" FORCE )

#############################################################################
# Marlin GUI
#############################################################################

# You need to set QT4_HOME path if you want to build the GUI
SET( MARLIN_GUI ON CACHE BOOL "Set to ON to build Marlin GUI" FORCE )

# Path to QT4 (needed for MARLIN_GUI)
SET( QT4_HOME "/afs/desy.de/group/it/ilcsoft/QT/4.2.2"
    CACHE PATH "Path to QT4" FORCE )
 
#############################################################################
# Optional packages
#############################################################################

# if you want to build and link Marlin with one or more of these additional
# tools you also have to define the corresponding "home" paths
SET( BUILD_WITH "GEAR LCCD CondDBMySQL CLHEP RAIDA"
    CACHE STRING "Build Marlin with these optional packages" FORCE )

# Path to GEAR
SET( GEAR_HOME "${ILC_HOME}/gear/v00-07"
    CACHE PATH "Path to GEAR" FORCE )

# Path to LCCD
SET( LCCD_HOME "${ILC_HOME}/lccd/v00-03-06"
    CACHE PATH "Path to LCCD" FORCE )

# Path to CondDBMySQL
SET( CondDBMySQL_HOME "${ILC_HOME}/CondDBMySQL/CondDBMySQL_ILC-0-5-10"
    CACHE PATH "Path to CondDBMySQL" FORCE )

# Path to CLHEP
SET( CLHEP_HOME "${ILC_HOME}/CLHEP/2.0.2.2"
    CACHE PATH "Path to CLHEP" FORCE )

# Path to RAIDA
SET( RAIDA_HOME "${ILC_HOME}/RAIDA/v01-03"
    CACHE PATH "Path to RAIDA" FORCE )

#############################################################################
# Link Marlin binary with Processor libraries
#############################################################################

# If you don't want to load processor libraries dynamically at run time with
# the environment variable MARLIN_DLL you can instead link the Marlin binary
# at compile time with these libraries.

# Define here what packages you want Marlin to be linked with
#SET( LINK_WITH "MarlinReco CEDViewer PandoraPFA SiliconDigi LCFIVertex"
#    CACHE STRING "Link Marlin with these optional packages" FORCE )
#
## Path to MarlinReco
#SET( MarlinReco_HOME "${ILC_HOME}/MarlinReco/HEAD"
#    CACHE PATH "Path to MarlinReco" FORCE )
#
## Path to CEDViewer
#SET( CEDViewer_HOME "${ILC_HOME}/CEDViewer/HEAD"
#    CACHE PATH "Path to CEDViewer" FORCE )
#
## Path to PandoraPFA
#SET( PandoraPFA_HOME "${ILC_HOME}/PandoraPFA/HEAD"
#    CACHE PATH "Path to PandoraPFA" FORCE )
#
## Path to SiliconDigi
#SET( SiliconDigi_HOME "${ILC_HOME}/SiliconDigi/HEAD"
#    CACHE PATH "Path to SiliconDigi" FORCE )
#
## Path to LCFIVertex
#SET( LCFIVertex_HOME "${ILC_HOME}/LCFIVertex/HEAD"
#    CACHE PATH "Path to LCFIVertex" FORCE )

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

# You can only a static Marlin binary under Linux!!!
#SET( BUILD_SHARED_LIBS OFF CACHE BOOL "Set to OFF to build static libraries" FORCE )

#SET( MARLIN_NO_DLL ON CACHE BOOL "Set to ON to build Marlin without DLL support" FORCE )

# installation path for Marlin
#SET( CMAKE_INSTALL_PREFIX "/foo/bar" CACHE STRING "Where to install Marlin" FORCE )
