#####################################################
#
# Example environment script to use AIDA with MARLIN
#  
#  F. Gaede 2004-11-15
#####################################################


export MARLIN_USE_AIDA=1

# modify the following pathes as needed
export JDK_HOME=/opt/products/java/1.4.2

export JAIDA_HOME=/opt/products/JAIDA/3.2.3

export AIDAJNI_HOME=/opt/products/AIDAJNI/3.2.3

. $JAIDA_HOME/bin/aida-setup.sh
. $AIDAJNI_HOME/bin/Linux-g++/aidajni-setup.sh

