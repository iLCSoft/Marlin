# v01-15-02

* 2017-11-22 Ete Remi ([PR#26](https://github.com/ilcsoft/Marlin/pull/26))
  - Added specific xml tree parsing to remove the output steering file name from the xml tree after parsing
    - avoid to re-generate repeatably parsed steering files after a first parsing

# v01-15-01

* 2017-11-15 Ete Remi ([PR#25](https://github.com/ilcsoft/Marlin/pull/25))
  - remove exception thrown in ProcessorMgr::processRunHeader() if gear detector name is different from the one in the lcio file, and just warn the user

# v01-15

* 2017-11-10 Ete Remi ([PR#24](https://github.com/iLCSoft/Marlin/pull/24))
  - EventSelector inherits from EventModifier and call processEvent() from modifyEvent()
  - Added safety clear of conditions in ProcessorMgr::modifyEvent

* 2017-11-10 Ete Remi ([PR#23](https://github.com/iLCSoft/Marlin/pull/23))
  - Added constants replacement for condition attributes in the execute section
    - Allows permanent conditions at runtime (not depending on processor return values)
    - Allows to refer to constants in conditions in the execute section, e.g : 
  ```xml
  <constants>
    <constant name="RunOverlay" value="false" />
  </constants>
  
  <execute>
    <if condition="${RunOverlay}">
      <processor name="MyOverlayBg"/>
    </if>
  </execute>
  ```
  and run : 
  ```shell
  Marlin steering.xml --constant.RunOverlay=true
  ```
  to change the behavior.

# v01-14

* 2017-11-02 Ete Remi ([PR#22](https://github.com/ilcsoft/Marlin/pull/22))
  - Added write() method to XMLParser (resp. Parser) to write the loaded xml tree (resp. same file) in a new file.
  - Many methods adapted in XMLParser to make parameter/constant replacements persistent in the XML tree
  - New global parameter "OutputSteeringFile" introduced to write down a new steering file after processing
  - Added command line option -n for a Marlin dry-run
  - Example : 
  ```shell
  # take the output file name from steering file and run Marlin as usual
  Marlin steeringFile.xml 
  # same but exit after writing the output steering file
  Marlin -n steeringFile.xml 
  # change the output file name
  Marlin steeringFile.xml --global.OutputSteeringFile=MyParsedSteeringFile.xml 
  ```

# v01-13

* 2017-09-27 Ete Remi ([PR#20](https://github.com/iLCSoft/Marlin/pull/20))
  - Added a constants section in XMLParser parsing : 
     - Allows to write constants and refer to in processor parameter values and global parameter values
     - Makes use of ${constantName} to refer to a constant
     - Use command line argument to override a constant
     - example:
  ```xml
    <constants>
      <constant name="MCParticleCollection" value="MCParticle"/>
      <constant name="FilePath" value="../../test/testmarlin"/>
      <constant name="InputFile" value="${FilePath}/simjob.slcio"/>
    </constants>
  ```
  ```shell
  Marlin --constant.InputFile=aDifferentFile.slcio  marlin.xml
  ```

* 2017-07-10 Andre Sailer ([PR#18](https://github.com/iLCSoft/Marlin/pull/18))
  - AIDAProcessor: delete file (_tree) at the end
  - LCIOOutputProcessor: delete lcWriter at the end
  - ProcessorMgr: clear maps and list at the end; delete static instance at the end

* 2017-09-25 Ete Remi ([PR#19](https://github.com/iLCSoft/Marlin/pull/19))
  - Added <include> element feature to read external xml file and replace in main Marlin steering file
  - Added an include mechanism to the `XMLParser` class, allowing to load xml files into the main Marlin steering file. Note that :
    - Nested includes will not be processed in the current version (include of include does not work)
     - Exceptions are thrown if something goes wrong with the xml element (wrong name, no file, etc ...)
     - Includes are processed iteratively in the document inside the root element.

* 2017-10-12 YancyW ([PR#21](https://github.com/iLCSoft/Marlin/pull/21))
  - add "copy_new_Processor.sh" and "action.sh" to the example folder. The former is for copying an old processor to the new one and it can also copy the example processor to user's directory. The latter for compiling the code. 
  - change the README.md, add usage for this two scripts.

# v01-12

* 2017-06-29 Andre Sailer ([PR#16](https://github.com/iLCSoft/Marlin/pull/16))
  - XMLParser: cleanup member objects, make StringParameters shared pointers to simplify ownership
     Previously some were owned by the Processors, others were still owned by the XMLParser object
     adapted interfaces
  - Processors: functions using `StringParameter*` now use or return `shared_ptr<StringParameters>` instead

* 2017-06-09 Andre Sailer ([PR#12](https://github.com/iLCSoft/Marlin/pull/12))
  - EventSeeder: make event seed truly constant in space and time

* 2017-06-14 Andre Sailer ([PR#13](https://github.com/iLCSoft/Marlin/pull/13))
  - Added MemoryMonitor processor to print out memory consumption of Marlin

* 2017-06-30 Frank Gaede ([PR#17](https://github.com/iLCSoft/Marlin/pull/17))
  - adapt MarlinGUi to new usage of shared_ptr<StringParameters>
   - remove duplicate entry in ReleaseNotes.md

* 2017-05-29 Andre Sailer ([PR#11](https://github.com/iLCSoft/Marlin/pull/11))
  - Add possibility to use conditions also for eventModify processors. The processor setting the conditions must also run under modifyEvent!

* 2017-05-29 Andre Sailer ([PR#10](https://github.com/iLCSoft/Marlin/pull/10))
  - compareMarlinSteeringFiles.py: add treatment for processor groups
  - compareMarlinSteeringFiles.py: add option to compare only selected processors

* 2017-06-23 Andre Sailer ([PR#15](https://github.com/iLCSoft/Marlin/pull/15))
  - Processor: throw exception if parameter is already defined

# v01-11

* 2017-02-27 Andre Sailer ([PR#8](https://github.com/iLCSoft/Marlin/pull/8))
  - Fix warnings for GCC
  - moved tinyXML to separate library

* 2016-12-07 Marko Petric ([PR#6](https://github.com/iLCSoft/Marlin/pull/6))
  - Made CI configuration generic and usable for all packages without modifications

* 2016-12-07 Andre Sailer ([PR#4](https://github.com/iLCSoft/Marlin/pull/4))
  - Add travis configuration to repository, enable testing for PRs

* 2016-11-25 Frank Gaede ([PR#3](https://github.com/iLCSoft/Marlin/pull/3))
  - Fix warnings for clang
  - ignore warnings from external headers


# v01-10

F.Gaede	

   - add a licence to marlin: GPLv3

S.Lu

  - fix buidling the MarlinGUI for gcc48 
    - apply '-DQT_NO_DEBUG' when 'MARLIN_GUI' is set to 'on' 
      and the MarlinGUI will be built.

D.Hynds

  - added escape behaviour mapped to SIGQUIT signal (^\). 
    Hitting this during running will run the end routine for each processor and then exit Marlin, 
    allowing you to stop event processing and still preserve histograms/results/etc. 
    ( A bug in LCIOOutputProcessor leads to a memory error on exiting, but has no impact 
      on anything else )


# v01-09

F. Gaede
   - made compatible with c++11
   - removed -ansi -pedantic -Wno-long-long
   - fix comparison of ostream and std::cout

A. Sailer
   - Add program to compare two marlinSteering files and print out differences between the two files based on parameter values installs the compareMarlinSteeringFiles.py into the Marlin bin folder, which is usually in the PATH

R. Simoniello
   - removed -Wall etc from mymarlin example for c++11

# v01-08

   M /Marlin/trunk/source/include/marlin/Processor.h
  - commented out old define for m_out (conflicts with a boost header )


# v01-07
     - introduced global flag: AllowToModifyEvent
       allowing to modify LCIO collections that have
       been read from an input file in the processEvent()
       method

# v01-06
    - added Statusmonitor processer (A.Sailer)
      - print run and event number every n-th event 
 

# v01-05

      - new optional global parameter: "LCIOReadCollectionNames"
        allows to restrict the collection that are read from the 
        LCIO file(s) to the ones given 
        ( needs LCIO v02-04 )

      - new command line option (H.Perrey, DESY):
         -u : consistency check and updated version of xml file

      - fix problem with ROOT seg fault at program end:
        do not call dlclose on shared libraries anymmore
        (A.Sailer)
        
# v01-04-01
   -  Ch. Rosemann: coverity bug fixes

# v01-04
   -  added EventSelector processor  
      - use Marlin -x and uncomment lines with EventSelector and <if>
        and specify run and event number in MyEventSelector

   -  bug fix: call Global::EVENTSEEDER->refreshSeeds( evt ) ;
      also for modifyEvent() which is needed for BG Overlay
   -  prevent implicit copy c'tors and assignment operators for Processors 

# v01-03
   - changed error handling:
     have global try-catch block in main in order to not have to rely on
     std::terminate/std::unexpeted (machine dependent and not compatible with ROOT)
   - fixed DEBUG printouts in startup phase (e.g. Marlin -x ...)


# v01-02-01
      - bug fixes: 
           - fixed new streamlog levels DEBUG5-DEBUG9, MESSAGE5-MESSAGE9,....ERROR9
	   - local streamlog level was not used in modifyEvent() and modifyRunHeader()
 
# v01-02
     - made command line options case sensitive

     - check spelling of command line options
       and enforce that parameters are present
       in steering file

     - increased verbosity threshold levels - added:
       DEBUG5-DEBUG9, MESSAGE5,....,ERROR9

# v01-01

    - Added dynamic command line options (see example by calling Marlin -h)
    - Added checking of duplicate processor entries in the steering file

    Bug fixes:
        - skip loading of duplicate lcio libraries/headers from lccd (cause warning on mac osx)
        - examples/mymarlin/src/MyProcessor.cc: protected against non-existing collections
        - made gcc 4.4.3 compliant
        - removed compiler warnings

# v01-00

        - Added a global random seed for Marlin processors. 
          This is used to provide individual pseudo-random numbers to Processors on
          an event-by-event and run-by-run basis. These may be used by Processors to 
	  seed their random number generator of choice. In order to use this service 
	  a Processor must register itself in the init method using:

                Global::EVENTSEEDER->registerProcessor(this);

          and should retrieve its allocated event seed during processEvent using:

                int eventSeed = Global::EVENTSEEDER->getSeed(this);

	  and include the header file:

	  	#include "marlin/ProcessorEventSeeder.h"	      	

          The global seed is used for a complete job and is set in the Global steering parameters thus:
        
                <parameter name="RandomSeed" value="1234567890"/>

          Note that the value must be a positive integer, with max value 2,147,483,647
          A pseudo-random event seed is generated using a three step hashing function of unsigned ints,
	  in the following order: event_number, run_number, RandomSeed. The hashed int from each step 
	  in the above order is used as input to the next hash step. This is used to ensure that in 
	  the likely event of similar values of event_number, run_number and RandomSeed, different 
	  event seeds will be generated. 
       
	  The event seed is then used to seed rand via srand(seed) and then rand is used to 
	  generate one seed per registered processor.

	  This mechanism ensures reproducible results for every event, regardless of the sequence 
	  in which the event is processed in a Marlin job, whilst maintaining the full 32bit range 
	  for event and run numbers.
          

        - allow RunHeaders to be modified in EventModifier interface 
	  (https://jira.slac.stanford.edu/browse/MAR-35)


	- added ./test directory with some initial marlin tests
          (use make test to run the tests)

        - improved CMake files and build procedure

            - added MarlinConfig.h to include general purpose configuration definitions
            - removed deprecated build mode: MARLIN_NO_DLL


 
# v00-13

        -  added macro streamlog_level( MLEVEL ) for use in if() statements with VERBOSITY, e.g.
          if( streamlog_level( DEBUG ) && x ) {
            // do additional computation
            // ...
            streamlog_out( DEBUG ) <<  ....
          }

        - fixes:
            AIDAProcessor:  switched default file extension to root
            StringParameters.cc: protect against converting strings like "12.3.4" to float 
            ProcessorLoader.cc : protect against loading duplicate libraries
	    
        - cmake changes/fixes:
            - CMakeLists.txt: bug fixed to generated documentation (overview.html)
            - BUILD_WITH not needed anymore to configure Marlin
            - simplified mymarlin example

# v00-12
     * new features:
       - processors can have local "Verbosity" parameter (overwrites global parameter for given processor)

       - processor return values now have to be set for every event, if they are used in a logical expression in the <execute/>
         section of the steering file - otherwise an exception is thrown (patch provided by Philipp Klenze, DESY)

   
       - removed (optional) dependency on CondDBMySQL
  
     * bug fixes:
        - exit Marlin if active Processor is undefined (i.e. no parameters defined for that processor)
        - made gcc 4.4.1 compliant

# v00-11

    * new features  (  JIRA issues MAR-37 MAR-43 MAR-45 )
           - Marlin stops processing if requested Processor does not exist
           - added const to Exception(const Processor*)
           - no unloading of plugins specified in MARLIN_DLL if MARLIN_DEBUG=1 is set 
             in environment
             -> useful for running Marlin jobs in valgrind for debugging

    * bug fixes:
       - added GEAR dependency. loading plugins could get undefined symbol: _ZN6marlin6Global4GEARE
       - gui: global parameter not saved if edit box still in focus
       - gui: applied patch to build on MAC


# v00-10-06


   * README: updated documentation (README)
   * bug fix: streamlog/source/include/streamlog/logbuffer.h:
         added #include <cstdio> for gcc 4.4.1


# v00-10-05

    * added BEFORE to INCLUDE_DIRECTORIES(...) statements in CMakeLists.txt
    * removed old GNU makefiles
    * removed old .steer example files
    

# v00-10-04

    * made cmake 2.6 compatible
    * added 32 bit compatibility build option
    * added LIBRARY_DIRS


# v00-10-03

      * added timing of Processor::processEvent() methods 
      -> printout at end of job


# v00-10-02


     * check detector model in gear file (if present) for consistency w/ 
	   detector name in lcio file 
	   stop processing if inconsistency found
 
     * add 'streamlog' like output for usage of plain  std::cout :
       [ VERBOSE "MyProcessor"]  my print message

     * added style and schema information to Marlin steering files
       (J.Engels/A.Vogel)
     
     * made compatible with gcc 4.3.x (Aplin/Bulgheroni)
   

# v00-10-01

      - inconsistent tag -> DON'T USE !


# v00-10-00

    * LCIOOutputProcesser:
      new parameters added: 
        KeepCollectionNames: collections are kept unconditionally 
        FullSubsetCollections: complete objects written to file - subset flag ignored
      needed/useful for 'DST' production

    * AIDAProcessor:
      added method AIDAProcessor::GetIAnalysisFactory(proc)

    * Marlin main:
      exit if a plugin fails to load

      bug fixes:
        - MarlinGUI used to crash when trying to open non-marlin xml file, e.g. a gear file
        - make valgrind happy by deleting 'global' objects
        - fixe problem with missing ${PROJECT_SOURCE_DIR}/doc/Doxyfile  
        - cmake: set INCLUDE_DIR and INCLUDE_DIRS in  MarlinConfig.cmake
        - allow multiple paths in CMAKE_MODULE_PATH



# v00-09-10


     - updated documentation
     - improved cmake files
     - removed dependency on HepPDT 
       ( requires CMakeModules v01-05 )



# v00-09-09

    - introduced EventModifier interface for modifying the LCIO input (e.g. for overlay)

    - printParameters() now on MESSAGE level - or templated: printParameters<DEBUG>() ;

    - bug fixes
	 gui: bug fixed in changing the gear file
         gui: bug fix with defining MARLIN_NO_DLL (Adrian Vogel)
         fixed documentation for streamlog
         for streamlog in templates (gcc3.2) use  streamlog_out_T( DEBUG ) 
         fixed verbosity level SILENT
         ...

   - made compatible with CLHEP versions >=1.9.3.0/2.0.3.0 (Martin Killenberg)


# v00-09-08


  - CMake is now default build tool  - see README for details

  - improved logging mechanism - see TestProcessor.cc as example:

      #include "streamlog/streamlog.h"  // needed only in non processor files!  
      // ...
      streamlog_out( MESSAGE ) << " processing event " << evt->getEventNumber() 
               	               << std::endl ;
    

     + additional log levels: DEBUG0,...DEBUG4, MESSAGE0-4, WARNING0-4, ERROR0-4

     + Processor::message<T>() is deprecated 
   
     + streamlog is an independant library for logging that can be also be used in other projects 
  

  - plugin mechanism is now default 
    ( set MARLIN_NO_DLL to disable plugins at compile time )

  - bug fix in SimpleFastMCProcessor ( anti-neutrinos and collection names )

 
  - macro for checking marlin version :

    #ifdef MARLIN_VERSION_GE
     #if MARLIN_VERSION_GE( 0, 9, 8 )    
        stream_log( MESSAGE ) << " using some feature introduced in Marlin v00-09-08 " << std::endl ;
     #endif
    #endif





# v00-09-07


  - optionally create a .dot file with a flow chart that shows processors 
    with their input and output collections (author: Ben Jeffrery, Oxford)
    
  - new methods
    Processor::message<VERBOSITY_LEVEL>(  const std::string& mess) 
    to display a message depending on the Verbosity 
    level, one of DEBUG, MESSAGE, WARNING, ERROR 

  - optionaly a working directory ($MARLINWORKDIR) can be used to build
    Marlin and dependend packages (in packages directory) 
    if not set $MARLIN will be used

  - added support for cmake (experimental) 

  - support for runtime plugins (experimental) 
    shared libraries with processors that are specified in	
    $MARLIN_PROCESSOR_LIBS (comma separated list of full-paths to libs) 
    are loaded at program start up	



# v00-09-06

 - first release of MarlinGUI that helps to create/modify xml steering files
   interactively -> see $MARLIN/gui/README for details

 - new methods Processor::registerInput/OutputCollections() 
   for checking consistency 

 - new feature 'Marlin -c steer.xml' checks steering file
   for consistency (names and types of LCIO collections (author J.Engels) 

 - new feature 'Marlin -f old.xml steer.xml ' adds LCIO collection
   type information to old xml steering file for usage with Marlin -c

 - new feature 'Marlin -o old.steer steer.xml ' 
   convert old (deprecated!) steering file to xml steering file
     (you can also use the GUI to read in old steering files)


 - switched to  latest version of tinyxml:  2.5.2


# v00-09-05

  - introduced bool processor parameters
  - allow to skip first N events (global parameter: SkipNEvents) requires LCIO > v01-05
  


# v00-09-04

  - made compliant for use with RAIDA 
  - no new functionality, only minor fixes and documentation


# v00-09-03


 - beta code for split files if given size exceeded in LCIOOutputprocessor

 - bug fix in LCIOOutputprocessor for DropCollectionNames


# v00-09-02

  - updated documentation

  - some code cosmetics


# v00-09-01

 
  first usage of GEAR
  
  introduced userlib.gmk for USERINCLUDES and USERLIBS

# v00-09

  added Exceptions and Exception handler that prints error message when an
  uncaught exception is thrown

  added SkipEventException and StopProcessingException to skip the current 
  event or all following events 

  improved makefiles for Marlin:
     -	src/GNUmakefile creates marlin_includes.sh and marlin_libs.sh
        in order to build with the same libraries as have been used for 
        core Marlin
     -  new top level GNUmakefile that builds marlin together with 
        user packages containing processors in $MARLIN/packages
     -  modified example makefile for user packages
        $MARLIN/examples/mymarlin/src/GNUmakefile

  new (optional) XML steering files that provide new features:

     - conditions: Processor::processEvent will be called only if a condition
       expressed in terms of ProcessorNames as specified in the steering file is true

     - groups: processors can be grouped together so that they can share common variables
       and can be referred to in the <execute/> tag by the group name

     - TinyXml parser ( http://sourceforge.net/projects/tinyxml ) 
       that can be used to read arbitrary xml files

  create example steering files:  
    ./bin/Marlin -l > example.steer 
    ./bin/Marlin -x > example.xml 

  LCIOOutputProcessor allows to drop collections - needs LCIO v01-05

  new method Processor::isFirstEvent() - use instead of static bool isFirstEvent

  added SimpleFastMCProcessor - simple smearing Monte Carlo - see fastMC.steer for an example

  new DataSourceProcessor class that allows reading of non-LCIO data streams
 
  StdHepReader processor that creates an LCIO data stream from an stdhep file
  (patch by J.Samson for limiting the number of events (MaxRecordNumber))

  bug fixes:
   empty lines containing whitspaces or tabs are ignored now (no seg.fault)
   processors end() method now called in reverse order of initialization !

# v00-08

  improved API doc
  
  bug fixes in ConditionsProcessor

  improved example/mymarlin/src/GNUmakefile

# v00-07
    
  added ConditionsProcessor for LCCD
   
  added optional parameters to Processor
 
  read chain of LCIO input files in Marlin.cc
   
  fixed problems with registered vector parameters

# v00-06

  added examples directory that can serve as a template for user projects

  added Processr::printParameters()
 
  fixed Problem with default values of parameters 

  renamed Processr::dump() to Processr::printDescription()

# v00-05

  added Processr::check()  to be used for checkplots in reco processors
 
# v00-04    (changes wrt. to v00-03):
  
   * generates warning instead of error message if less than MaxRecordNumber records read

   * improved handling/documentation of steering parameters / modules

     - Marlin -l : lists all kown modules and their steering parameters
		   with description and default values
                   in a format that can be copied to a  steering file 

       -> use Processor::registerProcessorParameter(name, description, parameter, defaultVal ) ;
          to declare your parameters

     - leading whitespace allowed in steering file to improve readability


   * new AIDAProcessor for producing AIDA/root histograms:

    - handles AIDA file for all modules 
    - modules simply create and fill their histograms
    - uses JAIDA/AIDAJNI AIDA-implementation 
    - not mandatory - could still use root but preferred  for 'official' processors/modules

