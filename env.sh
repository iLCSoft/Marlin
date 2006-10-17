#######################################################
#
# Example environment script to build MarlinReco together with Marlin.
#
# - Before building Marlin Place this script in the ./Marlin directory,
#   replacing the original environment script "env.sh". 
# - Then edit this script to adopt the paths to fit for your system.
# - source the script to make the changes permanent: 
#
#   . ./env.sh 
#
# ! Be aware of the fact that all variables defined in this script 
#   overwrite existing environment variables of the same name in your shell.  
# - In addition the file "userlib.gmk.MarlinReco" is written to the
#   current directory, containing a set of paths to libraries and 
#   include files. In case you need additional libraries or include files,
#   modify this file. before building MarlinReco "userlib.gmk"
#   has to be replaced by "userlib.gmk.MarlinReco": 
# 
#   mv userlib.gmk userlib.gmk.old
#   mv userlib.gmk.MarlinReco userlib.gmk
# 
#  T. Kraemer, DESY
# 
#######################################################

# Clean environment
unset LCIO
unset MARLINDEBUG
unset CLHEP
unset CERNLIB_HOME
unset MARLIN_USE_AIDA
unset JDK_HOME
unset JAIDA_HOME
unset AIDAJNI_HOME
unset GSL_HOME

# modify the following pathes as needed

# path to LCIO is required
export LCIO=/afs/desy.de/group/it/ilcsoft/lcio/v01-08-vtx

#-- comment out for production
export MARLINDEBUG=1

#---- CLHEP ---------
#export CLHEP=/opt/products/CLHEP/1.8.2
export CLHEP=/opt/products/CLHEP/2.0.2.2

#---- GSL ----------
export GSL_HOME=/afs/desy.de/group/it/ilcsoft/gsl/1.6

#---- CERNLIB
export CERNLIB_HOME=/opt/products/cernlib/pro/lib

#---- GEAR ---------------
export GEAR=/afs/desy.de/group/it/ilcsoft/gear/v00-02

#---- AIDA setup ---------

#--- fixes a bug in aida_env.sh with zsh (provided by J.Samson) :
setopt shwordsplit > /dev/null 2>&1

export MARLIN_USE_AIDA=1

# modify the following pathes as needed
export JDK_HOME=/opt/products/java/1.4.2

export JAIDA_HOME=/opt/products/JAIDA/3.2.3

export AIDAJNI_HOME=/opt/products/AIDAJNI/3.2.3

# -----------------------------------------------------------------------------

. $JAIDA_HOME/bin/aida-setup.sh
. $AIDAJNI_HOME/bin/Linux-g++/aidajni-setup.sh

if [ $MARLIN ] ; then
 export MARLIN
else
 export MARLIN=$PWD
fi

echo "###############################################################################"
echo "#"
echo "# Settings used to build MarlinRECO:" 
echo "# ----------------------------------"
echo "#"
echo "# MARLIN set to:            "$MARLIN
echo "# LCIO set to:              "$LCIO
echo "# MARLINDEBUG set to:       "$MARLINDEBUG
echo "# CLHEP set to:             "$CLHEP
echo "# GSL_HOME set to:          "$GSL_HOME
echo "# CERNLIB_HOME set to:      "$CERNLIB_HOME
echo "# GEAR set to:              "$GEAR
echo "# MARLIN_USE_AIDA set to:   "$MARLIN_USE_AIDA
echo "# JDK_HOME set to:          "$JDK_HOME
echo "# JAIDA_HOME set to:        "$JAIDA_HOME
echo "# AIDAJNI_HOME set to:      "$AIDAJNI_HOME
echo "#"
echo "###############################################################################"

# Write file with user includes and libraries
# Only include sections if variables are set.

if [ $1 ] ; then 
   outputfile=$1
else
   outputfile=userlib.gmk.MarlinReco
fi 

if [ -f $outputfile ] ; then
  rm -f $outputfile
fi

cat > $outputfile << EOF 
##############################################################################
#
#  User includes and libraries for building Marlin  with additional packages
#
#  F. Gaede, T. Kraemer, DESY
#
#
# Add as many of the following alines as needed :
#
# USERINCLUDES += -I include_path
# USERLIBS += -L path_to_lib -lmylib
#
# To build the package, rename these files:
# mv userlib.gmk userlib.gmk.old
# mv userlibMarlinReco.gmk userlib.gmk
#
##############################################################################

EOF

echo "USERINCLUDES += -I "$MARLIN"/packages/MarlinUtil/include" >> $outputfile
echo "USERINCLUDES += -Df2cFortran" >> $outputfile

if [ $GSL_HOME ] ; then 
   echo "USERINCLUDES += -I "$GSL_HOME"/include" >> $outputfile
fi

echo >> $outputfile
echo "USERLIBS += -lg2c" >> $outputfile

if [ $CERNLIB_HOME ] ; then
   echo "USERLIBS += -L "$CERNLIB_HOME"/ -lmathlib -lkernlib" >> $outputfile
fi

if [ $GSL_HOME ] ; then 
    echo "USERLIBS += -L "$GSL_HOME"/lib -lgsl -lgslcblas" >> $outputfile
fi
