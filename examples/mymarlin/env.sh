#######################################################
#
# Example environment script to build example MyMarlin
#  
#  F. Gaede 2004-12-02
#######################################################



# modify the following pathes as needed

export LCIO=/afs/desy.de/group/it/ilcsoft/lcio/lccdv00-01

export MARLIN=~/marlin/devel/v00-07
#/afs/desy.de/group/it/ilcsoft/marlin/v00-07

export LCCD=/afs/desy.de/group/it/ilcsoft/lccd/v00-01

# to make full use of LCCD also use the conditions data base
# Note: if you don't want to use CondDBMySQL you also need a LCCD library that
# has been build without CondDBMySQL !
export CondDBMySQL=/afs/desy.de/group/it/ilcsoft/CondDBMySQL

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CondDBMySQL/lib


#---- AIDA setup ---------
#export MARLIN_USE_AIDA=1

export JDK_HOME=/opt/products/java/1.4.2

export JAIDA_HOME=/opt/products/JAIDA/3.2.3

export AIDAJNI_HOME=/opt/products/AIDAJNI/3.2.3

. $JAIDA_HOME/bin/aida-setup.sh
. $AIDAJNI_HOME/bin/Linux-g++/aidajni-setup.sh

#------------------------------------------
