#######################################################
#
# Example environment script to build example MyMarlin
#  
#  F. Gaede 2004-12-02
#######################################################



# modify the following pathes as needed

export LCIO=/opt/products/lcio/1.3.1

export MARLIN=/opt/products/marlin/0.6


#---- AIDA setup ---------
export MARLIN_USE_AIDA=1

export JDK_HOME=/opt/products/java/1.4.2

export JAIDA_HOME=/opt/products/JAIDA/3.2.3

export AIDAJNI_HOME=/opt/products/AIDAJNI/3.2.3

. $JAIDA_HOME/bin/aida-setup.sh
. $AIDAJNI_HOME/bin/Linux-g++/aidajni-setup.sh

#------------------------------------------
