

Quick steps to build the mymarlin example:
------------------------------------------

    . /path/to/ilcsoft/installation/v01-XX/init_ilcsoft.sh
    mkdir build
    cd build
    cmake -C $ILCSOFT/ILCSoft.cmake ..
    make install # this should create library: lib/libmymarlin.so



Quick steps to load mymarlin example into Marlin:
-------------------------------------------------

    export MARLIN_DLL=$PWD/lib/libmymarlin.so
    Marlin -x > mysteer.xml
    grep mymarlin mysteer.xml # should display ... Loading shared library ... (lib/libmymarlin.so)



Quick steps to change this example into your own Marlin plugin:
---------------------------------------------------------------

    1.) rename source files:
        
        mv include/MyProcessor.h include/ChooseAReasonableNameForThisClass.h
        mv src/MyProcessor.cc src/ChooseAReasonableNameForThisClass.cc

    2.) change CMakeLists.txt:

        PROJECT( ChooseAReasonableName )

        Check DEPENDENCIES for additional required / optional packages


    3.) compile your new plugin and load it into Marlin as described above




Infos and support:
------------------

iLCSoft general documentation:
http://ilcsoft.desy.de/portal/general_documentation/index_eng.html

CMake website:
http://www.cmake.org

The Linear Collider Forum:
http://forum.linearcollider.org

