# Marlin (Modular Analysis and Reconstruction for the LINear Collider )
[![Build Status](https://travis-ci.org/iLCSoft/Marlin.svg?branch=master)](https://travis-ci.org/iLCSoft/Marlin)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/11089/badge.svg)](https://scan.coverity.com/projects/ilcsoft-marlin)

The idea is that every computing task is implemented as a processor (module) that analyzes data in an LCEvent and creates additional output collections that are added to the event. The framework allows to define the processors (and their order) that are executed at runtime in a simple steering file. Via the steering file you can also define named parameters (string, float, int - single and arrays) for every processor as well as for the global scope. By using the framework users don't have to write any code that deals with the IO they simply write processors with defined callbacks, i.e. init(), processRunHeader(), processEvent(), end().

Marlin is distributed under the [GPLv3 License](http://www.gnu.org/licenses/gpl-3.0.en.html)

[![License](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl-3.0.en.html)


## Instructions for building Marlin with CMake:
`. path_to_ilcsoft_installation/v01-XX/init_ilcsoft.sh`

`mkdir build`

`cd build`

`cmake -C $ILCSOFT/ILCSoft.cmake ..`

`make install`
### Loading plugins/processors dynamically ( at run time ):
It is possible to load plugins dynamically at run-time into Marlin by setting the colon-separated environment variable `MARLIN_DLL` to a list of plugins:

Take mymarlin as an example:

`cd ./examples/mymarlin`

`mkdir build ; cd build`

`cmake -C $ILCSOFT/ILCSoft.cmake ..`

`make install`


`export MARLIN_DLL="$PWD/lib/libmymarlin.so"`

`Marlin -x`

If you have 2 or more libs you should separate them with a ':'

`$ export MARLIN_DLL="/path1/lib1.so:/path2/lib2.so"`


## Create a new Marlin Plugin / Processor:

`./examples/mymarlin can be used as a template for a new Marlin plugin`

simply copy the whole directory:

`cp -r $MARLIN/examples/mymarlin MyFastJetClustering` (or whatever your processor should be called...)

renaming the files source files, e.g.

`mv include/MyProcessor.h include/ChooseAReasonableNameForThisClass.h`

`mv src/MyProcessor.cc src/ChooseAReasonableNameForThisClass.cc`

change in CMakeLists.txt:

`PROJECT( MyFastJetClusteringProcessor )`

if needed check DEPENDENCIES for additional required / optional packages


compile your new plugin as described above for the mymarlin example under "Loading plugins/processors dynamically"

### A Quick way to create a new Marlin Processor

   find a script "copy_new_processor.sh" in the examples folder
   ` ./copy_new_processor.sh  new_processor_name`

   This script can be decomposed by following steps:

   - Copy an example processer that supplied by ILCSoft  with .cc and .h file into a new folder and change the processer.
   - Change the processor class name into the  new name!! 
   NOTE: this is important, or it may conflict with existed processors.
   - Put ./action.sh into the bin folder, change the PROJECTNAME in the action, and run
   ./bin/action.sh
   - When running this action.sh, six folders will be created, and all files will be put into their specific folder.

    |   folder   |  meaning                |
    |:----------:|:-----------------------:|
    |  build     |  all compling file      |
    |  src       |  source file            |
    |  include   |  head file              |
    |  xml       |  steering file          |
    |  lib       |  your processor library |
    |  bin       |  execute file           |

   - go to xml folder, a default steering file has been created, change this steering file with the processors you want, then run it with
   ` Marlin mysteer.xml`
   - in the next time, when you change something for this processor and need to recompile it, just run `./bin/action.sh`.


   The copy_new_processor.sh script can also be used for copy any other exist processor to a new one by
   ` ./copy_new_processor.sh  old_processor_name new_processor_name`
   then a new processor folder will be copied, the file name and the class name in the file will be changed to the new processor name automatically.

## License and Copyright
Copyright (C), Marlin Authors

Marlin is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License long with this program.  If not, see <http://www.gnu.org/licenses/>.
