#!/bin/bash

echo 
if [[ $# != 2 && $# != 0 ]] ; then 
	echo "Error: need 0/2 argument!"
	echo "Usage: ./copy_new_Processor.sh  [old_processor   new_processor]"
fi

basic_folder=$ILCSOFT/Marlin/v01-12/examples/mymarlin

if [[ $# == 2 ]] ; then 
	old_processor=$(basename "$1")
	new_processor=$(basename "$2")
	old_folder=$(dirname "${1}")"/"
	new_folder=$(dirname "${2}")"/"
elif [[ $# == 0 ]] ; then 
	old_processor=MyProcessor
	new_processor=mymarlin
	old_folder=$basic_folder"/"
	new_folder=mymarlin
fi



if [ -d ${new_folder}${new_processor}  ] ; then
	echo "Already have  ${new_processor},  please change a name!"
	echo "Usage: ./copy_new_Processor.sh    old_processor   new_processor"
	exit
fi


cp -r ${old_folder}/${old_processor}  ${new_folder}/${new_processor}

echo "the new folder is ${new_folder}/${new_processor}"


if [ -d ${new_folder}/${new_processor}  ] ; then
	echo "enter the new folder "
	echo
	cd ${new_folder}/${new_processor}
else
	echo "Error: can not create ${new_processor}, please check!"
	echo
	exit
fi


echo "checking the CMakeLists.txt."


if [ -a "CMakeLists.txt"  ] ; then
	echo "Already have CMakeList, editing necessary libraries ..."
	echo
else
	echo "no CMakeList,  copy it from the source..."
	echo
	source_cmake=${basic_folder}"/"CMakeLists.txt
	if [ ! -a ${source_cmake}  ] ; then
		echo "Error: no source CMakeLists.txt, please find a new CMake source."
		exit
	fi
	cp ${source_cmake} ./
	echo "editing necessary libraries ..."
fi

if [[ $# == 2 ]] ; then
	sed -i "9c\ PROJECT( ${new_processor} )" CMakeLists.txt
fi


lib_test=` grep "std=gnu++" CMakeLists.txt | sed -e "s/ //g" `
if [[ ${lib_test} == "" ]] ; then
	echo "adding c++ compiling condition ..."
	sed -i '58c\ ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=gnu++11" )' CMakeLists.txt
	sed -i '59c\ ADD_DEFINITIONS( "-Wno-long-long" )' CMakeLists.txt
fi

root_test=` grep "ROOT REQUIRED" CMakeLists.txt | sed -e "s/ //g" `
if [[ ${root_test} == "" ]] ; then
	echo "adding root libraries ..."
	sed -i '31a\ FIND_PACKAGE( ROOT REQUIRED )' CMakeLists.txt
	sed -i '32a\ INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )' CMakeLists.txt
	sed -i '33a\ LINK_LIBRARIES( ${ROOT_LIBRARIES} )' CMakeLists.txt
fi


lcio_test=` grep "LCIO REQUIRED" CMakeLists.txt | sed -e "s/ //g" `
if [[ ${lcio_test} == "" ]] ; then
	echo "adding lcio libraries ..."
	sed -i '34a\ FIND_PACKAGE( LCIO REQUIRED )' CMakeLists.txt
	sed -i '35a\ INCLUDE_DIRECTORIES( ${LCIO_INCLUDE_DIRS} )' CMakeLists.txt
	sed -i '36a\ LINK_LIBRARIES( ${LCIO_LIBRARIES} )' CMakeLists.txt
fi




echo 
echo "checking the source files."




if [ -d "src"  ] ; then
	echo "Already have  --src-- filter with the source file"
	echo 
else
	echo "no source filter, mkdir new src filter"
	mkdir src 
	if [ -a *.cc  ] ; then
		echo "Already have source file, move to src filter"
		mv *.cc ./src
	else
		echo "no source file, please put the *.cc files in the src folder or in the ${new_folder} folder"
		exit
	fi
fi


if [[ $# == 2 ]] ; then
	cd ./src/
	echo "if the old class name equals to the old folder name, or default MyProcessor, this script will change the name with the new one."
	if [ -a ${old_processor}.cc ];then
		mv ${old_processor}.cc ${new_processor}.cc
		for f in *.cc ; do
			sed -i "s/${old_processor}/${new_processor}/g" ` grep Processor -rl ./ `
		done
	elif [ -a MyProcessor.cc ];then
		mv MyProcessor.cc ${new_processor}.cc
		for f in *.cc ; do
			sed -i "s/MyProcessor/${new_processor}/g" ` grep Processor -rl ./ `
		done
	else 
		echo "the old class name does not equal to the old folder name, or default MyProcessor, please  change the name yourself."
	fi
	cd ..
fi


echo "checking the include files."

if [ -d "include"  ] ; then
	echo "Already have  --include-- filter with the header file"
	echo 
else
	echo "no header filter"
	mkdir include
	if [ -a *.h  ] ; then
		echo "Already have header file, move to head filter"
		mv *.h ./include
	else
		echo "no head file, please put the *.h files in the include folder or in the ${new_folder} folder"
		exit
	fi
fi

if [[ $# == 2 ]] ; then
	cd ./include/
	if [ -a ${old_processor}.h ];then
		mv ${old_processor}.h ${new_processor}.h.
		for f in *.h ; do
			sed -i "s/${old_processor}/${new_processor}/g" ` grep Processor -rl ./ `
		done
	elif [ -a MyProcessor.h ];then
		mv MyProcessor.h ${new_processor}.h
		for f in *.h ; do
			sed -i "s/MyProcessor/${new_processor}/g" ` grep Processor -rl ./ `
		done
	else 
		echo "the old class name does not equal to the old folder name, or default MyProcessor, please  change the name yourself."
	fi
	cd ..
fi


echo "checking the lib files."

if [ -d "lib"  ] ; then
	echo "Already have  --lib-- filter, deleting old files ..."
	echo 
	rm ./lib/*
else
	echo "no lib filter"
	echo 
	mkdir lib 
fi


echo "checking the build files."



if [ -d "build"  ] ; then
	echo "Already have  --build-- filter, deleting old files ..."
	echo 
	rm ./build -r
	mkdir build 
else
	echo "no build filter"
	echo 
	mkdir build 
fi


if [ -d xml ] ; then
	echo "Already have  --xml-- folder with the steering file"
	echo 
else
	echo "no steering folder"
	echo 
	mkdir xml 
fi

if [ -d bin ] ; then
	echo "Already have  --bin-- folder "
	echo 
else
	echo "no bin folder"
	echo 
	mkdir bin 
fi

if [ -a action.sh ]; then
	cp action.sh ./bin/
else 
	cp ${basic_folder}/../action.sh ./bin/
fi


cd ./build

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

cd ../

dll_test=` echo ${MARLIN_DLL} | grep "lib${new_processor}" `


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}"  )" && pwd  )"

SHELL_NAME=bashrc
echo "Try to write lib of the newprocessor into ${HOME}/.bashrc, if you use other shell, you should change SHELL_NAME yourself!"

if [[  $dll_test == ""  ]] ;then
	inform="dir=$DIR/../ \n PROJECTNAME=${PROJECTNAME}\n libname=$dir\"/lib/lib$PROJECTNAME\" \n export MARLIN_DLL=\${MARLIN_DLL}:\${libname}.so \n	echo \"install the lib to MARLIN_DLL ,  the lib is at  \${libname}.so\""
	if [ -a ${HOME}/.${SHELL_NAME} ] ; then
		echo -e $inform >> ${HOME}/.${SHELL_NAME}
		echo "now you should use \"source ${HOME}/.${SHELL_NAME}\" command to make your processor works."
	else
		echo "no such shell config file... please create one yourself."
	fi
else
	echo "The following is the keywords \"MARLIN_DLL\", Please check whether your processor is included! (Usually it is the last one)  if it is included, you can run Marlin with your processor now. " 
	echo ""
	echo ${MARLIN_DLL}
fi


echo "generating a default steering file in the xml folder, change it to your own steering file."
cd xml
Marlin -x > mysteer.xml
cd ..

echo "When you change your processor, your can use ./bin/action.sh to compile the code, the make information are preserved in build/make.output file."

echo 
echo "end"
echo 
