#!/bin/bash

if [ $#==1 ] ; then 
	PROJECTNAME=MyProcessor
elif [ $#==2 ] ; then 
	PROJECTNAME=$1"Processor"
else
	echo "usage: ./action.sh [Processor Name (will append \"Processor\" automatically)]"
	exit
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}"  )" && pwd  )"
home_folder=${DIR}/.. 

if [ -a ${home_folder}/CMakeLists.txt  ] ; then
	echo "Already have CMakeList"
else
	echo "no CMakeList"
	cp $ILCSOFT/Marlin/v01-12/examples/mymarlin/CMakeLists.txt ${home_folder} 
	CMAKE_FILE=${home_folder}/CMakeLists.txt
	sed -i "9c\ PROJECT( ${PROJECTNAME} )"                                 ${CMAKE_FILE}
	sed -i '58c\ ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=gnu++11" )'  ${CMAKE_FILE}
	sed -i '59c\ ADD_DEFINITIONS( "-Wno-long-long" )'                      ${CMAKE_FILE}
	sed -i '31a\ FIND_PACKAGE( ROOT REQUIRED )'                            ${CMAKE_FILE}
	sed -i '32a\ INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )'              ${CMAKE_FILE}
	sed -i '33a\ LINK_LIBRARIES( ${ROOT_LIBRARIES} )'                      ${CMAKE_FILE}
	sed -i '34a\ FIND_PACKAGE( LCIO REQUIRED )'                            ${CMAKE_FILE}
	sed -i '35a\ INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS} )'              ${CMAKE_FILE}
	sed -i '36a\ LINK_LIBRARIES( ${LCIO_LIBRARIES} )'                      ${CMAKE_FILE}
fi

if [ -d ${home_folder}/build  ] ; then
	echo "Already have  --build-- folder to compile"
else
	echo "no build folder"
	mkdir ${home_folder}/build 
fi

echo 

if [ -d ${home_folder}/src  ] ; then
	echo "Already have  --src-- folder with the source file"
else
	echo "no source folder, mkdir new src folder"
	mkdir ${home_folder}/src 
	if [ -a ${home_folder}/*.cc  ] ; then
		echo "Already have source file, move to src folder"
		mv ${home_folder}/*.cc ${home_folder}/src
	else
		echo "no source file, stop"
		exit
	fi
fi

echo 

if [ -d ${home_folder}/include  ] ; then
	echo "Already have  --include-- folder with the header file"
else
	echo "no header folder"
	mkdir include
	if [ -a ${home_folder}/*.h  ] ; then
		echo "Already have header file, move to head folder"
		mv ${home_folder}/*.h ${home_folder}/include
	else
		echo "no head file, stop"
		exit
	fi
fi

echo 

if [ -d ${home_folder}/xml ] ; then
	echo "Already have  --xml-- folder with the steering file"
else
	echo "no steering folder"
	mkdir xml 
fi

echo 

cd ${home_folder}/build

if [ -a "make.output"  ] ; then
	rm make.output
fi

echo "begin to config"
echo 
cmake -C $ILCSOFT/ILCSoft.cmake ..    &> make.output                                                                 
wait
echo "begin to make"
echo 
make &> make.output
wait
echo "begin to make install"
echo 
make install &> make.output
ERRORMESSAGE=$(grep "error" -irn ./make.output)
echo "check the error message"
echo 
echo ${ERRORMESSAGE} 


if [[ ${ERRORMESSAGE} != "" ]]; then
	echo 
	echo "Notice, Error found!"
	exit
else
	echo "no error, export processor to system..."
fi

dll_test=` echo ${MARLIN_DLL} | grep "lib${PROJECTNAME}" `


cd ../

echo "Check whether your Marlin processor has been written into .bashrc or not,  if you don't need that processor please delete them from .bashrc. " 
echo

if [[  $dll_test == ""  ]] ;then
	inform="dir=$DIR/../ \n PROJECTNAME=${PROJECTNAME}\n libname=$dir\"/lib/lib$PROJECTNAME\" \n export MARLIN_DLL=\${MARLIN_DLL}:\${libname}.so \n	echo \"install the lib to MARLIN_DLL ,  the lib is at  \${libname}.so\""
	echo -e $inform >> ${HOME}/.bashrc
	echo "now you should use \"source ~/.bashrc\" command to make your processor works."
else
	echo "The following is the keywords \"MARLIN_DLL\", Please check whether your processor is included! (Usually it is the last one)  if it is included, you can run Marlin with your processor now. " 
	echo ""
	echo ${MARLIN_DLL}
fi

echo 
echo "end"
