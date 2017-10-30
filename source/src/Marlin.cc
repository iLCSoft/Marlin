#include "lcio.h"



#ifdef LCIO_MAJOR_VERSION 
#if LCIO_VERSION_GE( 1,2)  
#include "LCIOSTLTypes.h"
#endif
#else
#include "MarlinLCIOSTLTypes.h"
#endif

#include "marlin/ProcessorMgr.h"
#include "marlin/Processor.h"
#include "marlin/ProcessorEventSeeder.h"
#include "marlin/Exceptions.h"
#include "IO/LCReader.h"

#include "marlin/Parser.h"
#include "marlin/XMLParser.h"

#include "marlin/Global.h"

#include "marlin/MarlinSteerCheck.h"
#include "marlin/XMLFixCollTypes.h"

#include <sstream>
#include <fstream>
#include <string>
#include <assert.h>
#include <signal.h>

#include <cstring>
#include <algorithm>
#include <memory>

#include "gearimpl/Util.h"
#include "gearxml/GearXML.h"
#include "gearimpl/GearMgrImpl.h"

#include "marlin/ProcessorLoader.h"

#include "marlin/VerbosityLevels.h"
#include "streamlog/streamlog.h"

using namespace lcio ;
using namespace marlin ;
using namespace std ;


void createProcessors( Parser&  parser ) ;
// void createProcessors( XMLParser&  parser ) ;
void  createProcessors( const IParser&  parser) ;

void listAvailableProcessors() ;
void listAvailableProcessorsXML() ;
int printUsage() ;


// Handle user interruption
// This allows you to ^\ at any point to exit in a controlled way
void userException(int sig){
  std::cout<<std::endl<<"User interrupted with sig="<< sig <<std::endl;
	ProcessorMgr::instance()->end() ;
  exit(1);
}

/** LCIO framework that can be used to analyse LCIO data files
 *  in a modular way. All tasks have to be implemented in Subclasses
 *  of Processor. They will be called in the order specified in the steering file.
 *
 */
int main(int argc, char** argv ){

  // Register escape behaviour
  signal(SIGQUIT, userException);

  // ---- catch all uncaught exceptions in the end ...
  try{
 
    
    if( argc > 1 ){
        if( std::string(argv[1]) == "-x" ){
            std::cout  << "<?xml version=\"1.0\" encoding=\"us-ascii\"?>" << std::endl
                << "<!-- ?xml-stylesheet type=\"text/xsl\" href=\"http://ilcsoft.desy.de/marlin/marlin.xsl\"? -->" << std::endl
                << "<!-- ?xml-stylesheet type=\"text/xsl\" href=\"marlin.xsl\"? -->" << std::endl << std::endl;
        }
    }

    //#ifndef MARLIN_NO_DLL

    //------ load shared libraries with processors ------

    StringVec libs ;
    LCTokenizer tk1( libs, ':' ) ;

    std::string marlinProcs("") ;

    char * var =  getenv("MARLIN_DLL" ) ;

    if( var != 0 ) {
        marlinProcs = var ;
    } else {
        std::cout << std::endl << "<!-- You have no MARLIN_DLL variable in your environment "
            " - so no processors will be loaded. ! --> " << std::endl << std::endl ;
    }

    std::for_each( marlinProcs.begin(), marlinProcs.end(), tk1 ) ;

    ProcessorLoader loader( libs.begin() , libs.end()  ) ;
    if( loader.failedLoading() ){
        return(1);
    }

    //------- end processor libs -------------------------

    //#endif


    const char* steeringFileName = "none"  ;

    //map<string, map<string,string> > cmdlineparams; 
    CommandLineParametersMap cmdlineparams; 

    // check for dynamic command line arguments
    for( int i = 1 ; i < argc ; i++ ) {
        // cout << "argv[" << i << "]:\t" << argv[i] << endl ;

        if( string( argv[i] ).substr( 0, 2 ) == "--" ){
            // cout << "dynamic opt:\t" << string( argv[i] ).substr( 2 ) << endl ;

            // split dynamic argument by '=', i.e.
            // --global.LCIOInputFiles="1.slcio 2.slcio 3.slcio" --> global.LCIOInputFiles , "1.slcio 2.slcio 3.slcio"
            StringVec cmdlinearg, cmdlinekey ;
            LCTokenizer t( cmdlinearg, '=' ) ;

            string param( argv[i] ) ;

            string s( param.substr( 2 ) ) ;
            for_each( s.begin(), s.end(), t ) ;

            // cout << "split opt:\tkey: " << cmdlinearg[0] << ", value: " << cmdlinearg[1] << endl ;

            if( cmdlinearg.size() != 2 ){
                cerr << endl << "*** invalid command line option: " << argv[i] << endl << endl;
                return printUsage();
            }

            // split first arg by '.'
            // --global.LCIOInputFiles --> global , LCIOInputFiles
            s = cmdlinearg[0] ;
            LCTokenizer t2( cmdlinekey, '.' ) ;

            for_each( s.begin(), s.end(), t2 ) ;

            if( cmdlinekey.size() != 2 ){
                cerr << endl << "*** invalid command line option: " << argv[i] << endl << endl;
                return printUsage();
            }

            // // case insensitive command line options
            // std::transform(cmdlinekey[0].begin(), cmdlinekey[0].end(), cmdlinekey[0].begin(), ::toupper);
            // std::transform(cmdlinekey[1].begin(), cmdlinekey[1].end(), cmdlinekey[1].begin(), ::toupper);

            // cout << "split key:\tindex1: " << cmdlinekey[0] << ", index2: " << cmdlinekey[1] << endl ;

            // save dynamic options into map
            cmdlineparams[ cmdlinekey[0] ][ cmdlinekey[1] ] = cmdlinearg[1] ;

            std::string type = cmdlinekey[0] == "constant" ? "constant" : "parameter" ;
            cout << "<!-- steering file " << type << ": [ " << cmdlinekey[0] << "." << cmdlinekey[1] << " ] will be OVERWRITTEN with value: [\"" << cmdlinearg[1] << "\"] -->" << endl;

            // erase dynamic options from **argv
            for( int j = i ; j < argc-1 ; j++ ){
                argv[j] = argv[j+1];
            }
            argc--;
            i--;
        }
    }

    cout << endl ;

    // read file name from command line
    if( argc > 1 ){

        if( std::string(argv[1]) == "-l" ){
            listAvailableProcessors() ;
            return(0) ;
        }
        else if( std::string(argv[1]) == "-x" ){
            listAvailableProcessorsXML() ;
            return(0) ;
        }
        else if( std::string(argv[1]) == "-c" ){
            if( argc == 3 ){
                MarlinSteerCheck msc(argv[2], &cmdlineparams );
                msc.dump_information();
                return(0) ;
            }
            else{
                std::cout << "  usage: Marlin -c steeringFile.xml" << std::endl << std::endl;
                return(1);
            }
        }
	else if( std::string(argv[1]) == "-u" ){
	  if( argc == 4 ){
	    MarlinSteerCheck msc(argv[2], &cmdlineparams );
	    msc.dump_information();
	    return(msc.saveAsXMLFile(argv[3] )) ;
	  }
	  else{
	    std::cout << "  usage: Marlin -u oldsteering.xml newsteering.xml" << std::endl << std::endl;
	    return(1);
	  }
	}
        else if( std::string(argv[1]) == "-o" ){
            if( argc == 4 ){
                MarlinSteerCheck msc(argv[2], &cmdlineparams );
                msc.saveAsXMLFile(argv[3]) ;
                return(0) ;
            }
            else{
                std::cout << "  usage: Marlin -o old.steer new.xml" << std::endl << std::endl;
                return(1);
            }
        }
        else if( std::string(argv[1]) == "-f" ){
            if( argc == 4 ){
                XMLFixCollTypes fixColTypes(argv[2]);
                fixColTypes.parse(argv[3] );
                return(0) ;
            }
            else{
                std::cout << "  usage: Marlin -f oldsteering.xml newsteering.xml" << std::endl << std::endl;
                return(1);
            }
        }
        else if( std::string(argv[1]) == "-d" ){
            if( argc == 4 ){
                MarlinSteerCheck msc(argv[2], &cmdlineparams );
                msc.saveAsDOTFile(argv[3]);
                return(0) ;
            }
            else{
                std::cout << "  usage: Marlin -d steer.xml diagram.dot" << std::endl << std::endl;
                return(1);
            }
        }
        else if( std::string(argv[1]) == "-p" || std::string(argv[1]) == "-n" ){
          if( argc == 4 ){
            std::unique_ptr<XMLParser> parser = std::unique_ptr<XMLParser>( new XMLParser(argv[3]) ) ;
            // tell parser to take into account any options defined on the command line
            parser->setCmdLineParameters( cmdlineparams ) ;
            parser->parse();
            parser->write( argv[2] );
            
            // option -p : write and exit
            if( std::string(argv[1]) == "-n" )
              return(0);
            // option -n : write and steering file for further processing
            else
              steeringFileName = argv[3] ;
          }
          else{
              std::cout << "  usage: Marlin " << argv[1] << " poststeering.xml steering.xml" << std::endl << std::endl;
              return(1);
          }
        }
        else if( std::string(argv[1]) == "-h"  || std::string(argv[1]) == "-?" ){

            return printUsage() ;
        }
        else{
            // one argument given: the steering file for normal running :
            steeringFileName = argv[1] ;          
        }

    } else {

        return printUsage() ;
    }


    //###### init streamlog ######
    std::string binname = argv[0]  ;
    binname = binname.substr( binname.find_last_of("/") + 1 , binname.size() ) ;
    streamlog::out.init( std::cout , binname ) ;



    std::unique_ptr<IParser> parser;

    // for now allow xml and old steering
    std::string filen(  steeringFileName ) ;

    if( filen.rfind(".xml") == std::string::npos ||  // .xml not found at all
            !(  filen.rfind(".xml")
                + strlen(".xml") == filen.length() ) ) {  
        parser = std::unique_ptr<IParser> ( new Parser( steeringFileName ) );

    } else {

        parser = std::unique_ptr<IParser>( new XMLParser(steeringFileName) ) ;

        // tell parser to take into account any options defined on the command line
        parser->setCmdLineParameters( cmdlineparams ) ;

    }

    parser->parse() ;

    Global::parameters = parser->getParameters("Global").get();

    //fg: can't use assert, as this generates no code when compiled with NDEBUG
    if( Global::parameters  == 0 ) {
        std::cout << "  Could not get global parameters from steering file ! " << std::endl  
            << "   The program has to exit - sorry ! " 
            << std::endl ;
        return(1) ;
    }


    // //-----  register log level names with the logstream ---------
    streamlog::out.addLevelName<DEBUG>() ;
    streamlog::out.addLevelName<DEBUG0>() ;
    streamlog::out.addLevelName<DEBUG1>() ;
    streamlog::out.addLevelName<DEBUG2>() ;
    streamlog::out.addLevelName<DEBUG3>() ;
    streamlog::out.addLevelName<DEBUG4>() ;
    streamlog::out.addLevelName<DEBUG5>() ;
    streamlog::out.addLevelName<DEBUG6>() ;
    streamlog::out.addLevelName<DEBUG7>() ;
    streamlog::out.addLevelName<DEBUG8>() ;
    streamlog::out.addLevelName<DEBUG9>() ;
    streamlog::out.addLevelName<MESSAGE>() ;
    streamlog::out.addLevelName<MESSAGE0>() ;
    streamlog::out.addLevelName<MESSAGE1>() ;
    streamlog::out.addLevelName<MESSAGE2>() ;
    streamlog::out.addLevelName<MESSAGE3>() ;
    streamlog::out.addLevelName<MESSAGE4>() ;
    streamlog::out.addLevelName<MESSAGE5>() ;
    streamlog::out.addLevelName<MESSAGE6>() ;
    streamlog::out.addLevelName<MESSAGE7>() ;
    streamlog::out.addLevelName<MESSAGE8>() ;
    streamlog::out.addLevelName<MESSAGE9>() ;
    streamlog::out.addLevelName<WARNING>() ;
    streamlog::out.addLevelName<WARNING0>() ;
    streamlog::out.addLevelName<WARNING1>() ;
    streamlog::out.addLevelName<WARNING2>() ;
    streamlog::out.addLevelName<WARNING3>() ;
    streamlog::out.addLevelName<WARNING4>() ;
    streamlog::out.addLevelName<WARNING5>() ;
    streamlog::out.addLevelName<WARNING6>() ;
    streamlog::out.addLevelName<WARNING7>() ;
    streamlog::out.addLevelName<WARNING8>() ;
    streamlog::out.addLevelName<WARNING9>() ;
    streamlog::out.addLevelName<ERROR>() ;
    streamlog::out.addLevelName<ERROR0>() ;
    streamlog::out.addLevelName<ERROR1>() ;
    streamlog::out.addLevelName<ERROR2>() ;
    streamlog::out.addLevelName<ERROR3>() ;
    streamlog::out.addLevelName<ERROR4>() ;
    streamlog::out.addLevelName<ERROR5>() ;
    streamlog::out.addLevelName<ERROR6>() ;
    streamlog::out.addLevelName<ERROR7>() ;
    streamlog::out.addLevelName<ERROR8>() ;
    streamlog::out.addLevelName<ERROR9>() ;
    streamlog::out.addLevelName<SILENT>() ;


    //-------- init logging level ------------
    std::string verbosity = Global::parameters->getStringVal("Verbosity" ) ;
    streamlog::logscope scope( streamlog::out ) ;

    scope.setLevel( verbosity ) ;

    createProcessors( *parser ) ;


    //#ifdef USE_GEAR

    std::string gearFile = Global::parameters->getStringVal("GearXMLFile" ) ;

    if( gearFile.size() > 0 ) {
      


      // //---------- check if we have an extension Gear file -------------------------

      // StringVec gearExtFiles ; 

      // if( (Global::parameters->getStringVals("GearExtensionFiles" , gearExtFiles ) ).size() == 2 ){
	
      // 	streamlog_out( MESSAGE ) << " ======== Extension Gear file given: " <<  gearExtFiles[0] << " ============= \n"
      // 				 << "     will add its parameters to the original Gear file: "  << gearFile << "\n"
      // 				 << "     resulting Gear file will be used and saved as:     "  << gearExtFiles[1] << "\n"
      // 				 << " ========================================================================= " << std::endl ;
	
      // 	gear::MergeXML mergeXML ;
	
      // 	if( ! mergeXML.setFile1(  gearFile  )){
	  
      // 	  streamlog_out( ERROR ) << "  Could not read Gear file  " <<  gearFile << std::endl ;
      // 	  exit(1) ;
      // 	}
	
      // 	if( ! mergeXML.setFile2( gearExtFiles[0] )){
	  
      // 	  streamlog_out( ERROR ) << "  Could not read extension Gear file  " <<  gearExtFiles[0] << std::endl ;
      // 	  exit(1) ;
      // 	}
	
      // 	if( !  mergeXML.mergeFiles( gearExtFiles[1] )){
	  
      // 	  streamlog_out( ERROR ) << "  Could not write extended Gear file  " <<  gearExtFiles[1] << std::endl ;
      // 	  exit(1) ;
      // 	}
	
      // 	gearFile =  gearExtFiles[1] ;

      // }
      // //----------------------------------------------------------------------------
      
      gear::GearXML gearXML( gearFile ) ;
      
      Global::GEAR = gearXML.createGearMgr() ;
      
      streamlog_out( MESSAGE )  << " ---- instantiated  GEAR from file  " << gearFile  << std::endl 
				<< *Global::GEAR << std::endl ;
      
    } else {

        streamlog_out( WARNING ) << " ---- no GEAR XML file given  --------- " << std::endl ;
        Global::GEAR = new gear::GearMgrImpl ;
    }

    //#endif

    StringVec lcioInputFiles ; 

    if ( (Global::parameters->getStringVals("LCIOInputFiles" , lcioInputFiles ) ).size() == 0 ){

        int maxRecord = Global::parameters->getIntVal("MaxRecordNumber");
        ProcessorMgr::instance()->init() ; 
        // fixme: pass maxRecord-1 (because of the runheader, which is generated)?
        ProcessorMgr::instance()->readDataSource(maxRecord) ; 
        ProcessorMgr::instance()->end() ; 

    } else { 



        int maxRecord = Global::parameters->getIntVal("MaxRecordNumber") ;
        int skipNEvents = Global::parameters->getIntVal("SkipNEvents");

        bool modify = ( Global::parameters->getStringVal("AllowToModifyEvent") == "true" ) ;

	if( modify ) {

	  streamlog_out( WARNING )  << " ******************************************************************************* \n" 
				    << " *    AllowToModifyEvent is set to 'true'                                      * \n"
				    << " *    => all processors can modify the input event in processEvent() !!        * \n"
				    << " *        consider setting this flag to 'false'                                * \n"
				    << " *        unless you really need it...                                         * \n"
				    << " *    - if you need a processor that modifies the input event                  * \n"
				    << " *      please implement the EventModifier interface and use the modifyEvent() * \n"
				    << " *      method for this                                                        * \n"
 				    << " ******************************************************************************* \n" 
				    << std::endl ;
	}

        // create lcio reader 
        LCReader* lcReader = LCFactory::getInstance()->createLCReader() ;

	StringVec readColNames ; 
	if( (Global::parameters->getStringVals("LCIOReadCollectionNames" , readColNames ) ).size() != 0 ){
	  
	  streamlog_out( WARNING )  << " *********** Parameter LCIOReadCollectionNames given - will only read the following collections: **** " 
				    << std::endl ;

	  for( unsigned i=0,N=readColNames.size() ; i<N ; ++i ) {
	    streamlog_out( WARNING )  << "     " << readColNames[i] << std::endl ;
	  } 
	  streamlog_out( WARNING )  << " *************************************************************************************************** " << std::endl ;

#if  LCIO_PATCHVERSION_GE( 2,4,0 )

	  lcReader->setReadCollectionNames( readColNames ) ;
#endif
	} 

        lcReader->registerLCRunListener( ProcessorMgr::instance() ) ; 
        lcReader->registerLCEventListener( ProcessorMgr::instance() ) ; 

        ProcessorMgr::instance()->init() ; 

        bool rewind = true ;

        while( rewind ) {

            rewind = false ;

            // process the data
            lcReader->open( lcioInputFiles  ) ; 


            if( skipNEvents > 0 ){

                streamlog_out( WARNING ) << " --- Marlin.cc - will skip first " << skipNEvents << " event(s)" 
                    << std::endl << std::endl ;

                lcReader->skipNEvents(  skipNEvents ) ;
            }

            try{ 
                if( maxRecord > 0 ){

                    try{
                        lcReader->readStream( maxRecord ) ;
                    }
                    catch( lcio::EndOfDataException& e){

                        streamlog_out( WARNING ) << e.what() << std::endl ;
                    }

                } else {

                    lcReader->readStream() ;
                }


            } catch( StopProcessingException &e) {

                streamlog_out( ERROR )  << std::endl
                    << " **********************************************************" << std::endl
                    << " *                                                        *" << std::endl
                    << " *   Stop of EventProcessiong requested by processor :    *" << std::endl
                    << " *                  "  << e.what()                           << std::endl
                    << " *     will call end() method of all processors !         *" << std::endl
                    << " *                                                        *" << std::endl
                    << " **********************************************************" << std::endl
                    << std::endl ;

            } catch( RewindDataFilesException &e) {

                rewind = true ;

                streamlog_out( ERROR )  << std::endl
                    << " **********************************************************" << std::endl
                    << " *                                                        *" << std::endl
                    << " *   Rewind data files requested by processor :           *" << std::endl
                    << " *                  "  << e.what()                           << std::endl
                    << " *     will rewind to beginning !                         *" << std::endl
                    << " *                                                        *" << std::endl
                    << " **********************************************************" << std::endl
                    << std::endl ;
            }


            lcReader->close() ;

            if( !rewind ) {

                ProcessorMgr::instance()->end() ; 

                delete lcReader ;
            }

        } // end rewind

    }

    //#ifdef USE_GEAR  

    if(  Global::GEAR != 0 ) 
        delete Global::GEAR ; 

    //#endif  

    return 0 ;

  } catch( std::exception& e) {

    std::cerr << " ***********************************************\n" 
	      << " A runtime error occured - (uncaught exception):\n" 
	      << "      " <<    e.what() << "\n"
	      << " Marlin will have to be terminated, sorry.\n"  
	      << " ***********************************************\n" 
	      << std:: endl ; 

    return 1 ;

  }

}

//   void  createProcessors(XMLParser&  parser) {
void createProcessors( const IParser&  parser) {

    StringVec activeProcessors ;
    Global::parameters->getStringVals("ActiveProcessors" , activeProcessors ) ;

    StringVec procConds ;
    Global::parameters->getStringVals("ProcessorConditions" , procConds ) ;

    bool useConditions = ( activeProcessors.size() == procConds.size() ) ;

    //     for( StringVec::iterator m = activeProcessors.begin() ; m != activeProcessors.end() ; m++){
    for(unsigned int i=0 ; i<  activeProcessors.size() ; i++ ) {

        std::shared_ptr<StringParameters> p = parser.getParameters( activeProcessors[i] );

        if( p!=0 ){
            std::string type = p->getStringVal("ProcessorType") ;

            if( useConditions ) 
                ProcessorMgr::instance()->addActiveProcessor( type , activeProcessors[i] , p , procConds[i] )  ;
            else
                ProcessorMgr::instance()->addActiveProcessor( type , activeProcessors[i] , p )  ;

        } else{
            std::stringstream sstr ;
            sstr << "Undefined processor : " << activeProcessors[i] << std::endl ;
            streamlog_out( ERROR )  << sstr.str() ;	
            throw Exception( sstr.str() );
        }
    }
}

void  createProcessors(Parser&  parser) {

    StringVec activeProcessors ;
    Global::parameters->getStringVals("ActiveProcessors" , activeProcessors ) ;

    //for( StringVec::iterator m = activeProcessors.begin() ; m != activeProcessors.end() ; m++){
    for(unsigned int i=0 ; i<  activeProcessors.size() ; i++ ) {

        //std::shared_ptr<StringParameters> p = parser.getParameters( *m )  ;
        std::shared_ptr<StringParameters> p = parser.getParameters( activeProcessors[i] );


        streamlog_out( MESSAGE ) << " Parameters for processor " << activeProcessors[i]
            << std::endl 
            << *p ; 

        if( p!=0 ){
            std::string type = p->getStringVal("ProcessorType") ;

            //if( ProcessorMgr::instance()->addActiveProcessor( type , *m , p )  ){
            if( ProcessorMgr::instance()->addActiveProcessor( type , activeProcessors[i] , p )  ){

                // 	Processor* processor =  ProcessorMgr::instance()->getActiveProcessor( *m ) ;
                //	processor->setParameters( p ) ;
            }
        } else{
            std::stringstream sstr ;
            sstr << "Undefined processor : " << activeProcessors[i] << std::endl ;
            streamlog_out( ERROR )  << sstr.str() ;	
            throw Exception( sstr.str() );

        }

    }
}

void listAvailableProcessors() {

    ProcessorMgr::instance()->dumpRegisteredProcessors() ;
}

void listAvailableProcessorsXML() {

    ProcessorMgr::instance()->dumpRegisteredProcessorsXML() ;
}


int printUsage() {

  std::cout << " Usage: Marlin [OPTION] [FILE]..." << std::endl 
	    << "   runs a Marlin application " << std::endl 
	    << std::endl 
	    << " Running the application with a given steering file:" << std::endl 
	    << "   Marlin steer.xml   " << std::endl 
	    << std::endl 
	    << "   Marlin [-h/-?]             \t print this help information" << std::endl 
	    << "   Marlin -x                  \t print an example steering file to stdout" << std::endl 
	    << "   Marlin -c steer.xml        \t check the given steering file for consistency" << std::endl 
	    << "   Marlin -u old.xml new.xml  \t consistency check with update of xml file"  << std::endl
	    << "   Marlin -d steer.xml flow.dot\t create a program flow diagram (see: http://www.graphviz.org)" << std::endl 
	    << "   Marlin -l                  \t [deprecated: old format steering file example]" << std::endl 
	    << "   Marlin -f old.xml new.xml  \t [deprecated: convert old xml files to new xml files for consistency check]"  << std::endl 
	    << "   Marlin -o old.steer new.xml\t [deprecated: convert old steering file to xml steering file]" << std::endl 
	    << std::endl 
	    << " Example: " << std::endl 
	    << " To create a new default steering file from any Marlin application, run" << std::endl 
	    << "     Marlin -x > mysteer.xml" << std::endl 
	    << " and then use either an editor or the MarlinGUI to modify the created steering file " << std::endl 
	    << " to configure your application and then run it. e.g. : " << std::endl 
	    << "     Marlin mysteer.xml > marlin.out 2>&1 &" << std::endl << std::endl
	    << " Dynamic command line options may be specified in order to overwrite individual steering file parameters, e.g.:" << std::endl 
	    << "     Marlin --global.LCIOInputFiles=\"input1.slcio input2.slcio\" --global.GearXMLFile=mydetector.xml" << std::endl 
	    << "            --MyLCIOOutputProcessor.LCIOWriteMode=WRITE_APPEND --MyLCIOOutputProcessor.LCIOOutputFile=out.slcio steer.xml" << std::endl << std::endl
	    << "     NOTE: Dynamic options do NOT work together with Marlin options (-x, -f, -l) nor with the MarlinGUI or old steering files" << std::endl 
	    << std::endl ;
  
  return(0) ;

}

