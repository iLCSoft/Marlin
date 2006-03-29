#######################################################
#
# Example environment script to build Marlin
#  
#  F. Gaede, DESY
#######################################################


if [ $MARLIN ] ; then
 export MARLIN
else
 export MARLIN=$PWD
fi

echo "  MARLIN set to:" $MARLIN


# modify the following pathes as needed

# path to LCIO is required
#export LCIO=/opt/products/lcio/1.5
export LCIO=/afs/desy.de/group/it/ilcsoft/lcio/v01-07


#-- comment out for production 
export MARLINDEBUG=1


# the following is optional (but recommended) comment 
# out before compiling what you don't need/want

#---- CLHEP ---------
export CLHEP=/opt/products/CLHEP/1.8.2


#---- LCCD -----------

# use LCCD for conditions data (ConditionsProcessor) 
#export LCCD=/afs/desy.de/group/it/ilcsoft/lccd/v00-02

# to make full use of LCCD also use the conditions data base
# Note: if you don't want to use CondDBMySQL you also need a LCCD library that
# has been build without CondDBMySQL !

#export CondDBMySQL=/afs/desy.de/group/it/ilcsoft/CondDBMySQL

#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CondDBMySQL/lib


#---- GEAR ---------------
export GEAR=/afs/desy.de/group/it/ilcsoft/gear/v00-02


#---- AIDA setup ---------

#--- fixes a bug in aida_env.sh with zsh (provided by J.Samson) :
setopt shwordsplit > /dev/null 2>&1

export MARLIN_USE_AIDA=1

#----------- use RAIDA  -------------------------

export ROOTSYS=/opt/products/root/5.08.00
export RAIDA_HOME=/afs/desy.de/user/g/gaede/ilcsoft/RAIDA/v00-00

export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
export PATH=$ROOTSYS/bin:$PATH

. $RAIDA_HOME/bin/aida-setup.sh

#--------------  use JAIDA -----------------------
# modify the following pathes as needed
#export JDK_HOME=/opt/products/java/1.4.2
#export JAIDA_HOME=/opt/products/JAIDA/3.2.3
#export AIDAJNI_HOME=/opt/products/AIDAJNI/3.2.3

#. $JAIDA_HOME/bin/aida-setup.sh
#. $AIDAJNI_HOME/bin/Linux-g++/aidajni-setup.sh
#------------------------------------------
