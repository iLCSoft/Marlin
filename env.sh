#######################################################
#
# Example environment script to build Marlin
#  
#  F. Gaede 2005-02-23
#######################################################

# modify the following pathes as needed

# path to LCIO is required
export LCIO=/afs/desy.de/group/it/ilcsoft/lcio/lccdv00-01


#-- comment out for production 
export MARLINDEBUG=1


# the following is optional (but recommended) comment 
# out before compiling what you don't need/want

#---- LCCD -----------

# use LCCD for conditions data (ConditionsProcessor) 
export LCCD=/afs/desy.de/group/it/ilcsoft/lccd/v00-01

# to make full use of LCCD also use the conditions data base
# Note: if you don't want to use CondDBMySQL you also need a LCCD library that
# has been build without CondDBMySQL !
export CondDBMySQL=/afs/desy.de/group/it/ilcsoft/CondDBMySQL

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CondDBMySQL/lib
#-------------------------


#---- AIDA setup ---------

. aida_env.sh
#------------------------------------------
