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

 
#include <cstring>
#include <algorithm>

#ifdef USE_GEAR
#include "gearimpl/Util.h"
#include "gearxml/GearXML.h"
#include "gearimpl/GearMgrImpl.h"
#endif

#include "marlin/ProcessorLoader.h"

#include "marlin/VerbosityLevels.h"
#include "streamlog/streamlog.h"

using namespace lcio ;
using namespace marlin ;


void createProcessors( Parser&  parser ) ;
// void createProcessors( XMLParser&  parser ) ;
void  createProcessors( const IParser&  parser) ;

void listAvailableProcessors() ;
void listAvailableProcessorsXML() ;
int printUsage() ;


/** LCIO framework that can be used to analyse LCIO data files
 *  in a modular way. All tasks have to be implemented in Subclasses
 *  of Processor. They will be called in the order specified in the steering file.
 *
 */
int main(int argc, char** argv ){

  // this macro enables printout of uncaught exceptions
  HANDLE_LCIO_EXCEPTIONS
  

    //###### init streamlog ######
    std::string binname = argv[0]  ;
  binname = binname.substr( binname.find_last_of("/") + 1 , binname.size() ) ;
  streamlog::out.init( std::cout , binname ) ;
  
  if( argc > 1 ){
    if( std::string(argv[1]) == "-x" ){
       std::cout  << "<?xml version=\"1.0\" encoding=\"us-ascii\"?>" << std::endl
       << "<!-- ?xml-stylesheet type=\"text/xsl\" href=\"http://ilcsoft.desy.de/marlin/marlin.xsl\"? -->" << std::endl
       << "<!-- ?xml-stylesheet type=\"text/xsl\" href=\"marlin.xsl\"? -->" << std::endl << std::endl;
    }
  }

#ifndef MARLIN_NO_DLL
    
  //------ load shared libraries with processors ------
    
  StringVec libs ;
  LCTokenizer t( libs, ':' ) ;
  
  std::string marlinProcs("") ;
  
  char * var =  getenv("MARLIN_DLL" ) ;
  
  if( var != 0 ) {
    marlinProcs = var ;
  } else {
    std::cout << std::endl << "<!-- You have no MARLIN_DLL variable in your environment "
      " - so no processors will be loaded. ! --> " << std::endl << std::endl ;
  }
  
  std::for_each( marlinProcs.begin(), marlinProcs.end(), t ) ;
  
  ProcessorLoader loader( libs.begin() , libs.end()  ) ;
  if( loader.failedLoading() ){
    return(1);
  }
  
  //------- end processor libs -------------------------
  
#endif


  const char* steeringFileName = "none"  ;
  
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
	MarlinSteerCheck msc(argv[2]);
	msc.dump_information();
        return(0) ;
      }
      else{
	std::cout << "  usage: Marlin -c steeringFile.xml" << std::endl << std::endl;
	return(1);
      }
    }
    else if( std::string(argv[1]) == "-o" ){
      if( argc == 4 ){
	MarlinSteerCheck msc(argv[2]);
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
	MarlinSteerCheck msc(argv[2]);
	msc.saveAsDOTFile(argv[3]);
	return(0) ;
      }
      else{
	std::cout << "  usage: Marlin -d steer.xml diagram.dot" << std::endl << std::endl;
	return(1);
      }
    }
    else if( std::string(argv[1]) == "-h"  || std::string(argv[1]) == "-?" ){

      return printUsage() ;
    }


    // one argument given: the steering file for normal running :
    steeringFileName = argv[1] ;

  } else {

    return printUsage() ;
  }
  




  IParser* parser ;

  // for now allow xml and old steering
  std::string filen(  steeringFileName ) ;

  if( filen.rfind(".xml") == std::string::npos ||  // .xml not found at all
      !(  filen.rfind(".xml")
 	  + strlen(".xml") == filen.length() ) ) {  
    parser = new Parser( steeringFileName ) ;
    
  } else {
    
    parser = new XMLParser( steeringFileName ) ;
  }
  
  parser->parse() ;

  Global::parameters = parser->getParameters("Global")  ;

  //fg: can't use assert, as this generates no code when compiled with NDEBUG
  if( Global::parameters  == 0 ) {
    std::cout << "  Could not get global parameters from steering file ! " << std::endl  
	      << "   The program has to exit - sorry ! " 
	      << std::endl ;
    return(1) ;
  }
  

  //-----  register log level names with the logstream ---------
  streamlog::out.addLevelName<DEBUG>() ;
  streamlog::out.addLevelName<DEBUG0>() ;
  streamlog::out.addLevelName<DEBUG1>() ;
  streamlog::out.addLevelName<DEBUG2>() ;
  streamlog::out.addLevelName<DEBUG3>() ;
  streamlog::out.addLevelName<DEBUG4>() ;
  streamlog::out.addLevelName<MESSAGE>() ;
  streamlog::out.addLevelName<MESSAGE0>() ;
  streamlog::out.addLevelName<MESSAGE1>() ;
  streamlog::out.addLevelName<MESSAGE2>() ;
  streamlog::out.addLevelName<MESSAGE3>() ;
  streamlog::out.addLevelName<MESSAGE4>() ;
  streamlog::out.addLevelName<WARNING>() ;
  streamlog::out.addLevelName<WARNING0>() ;
  streamlog::out.addLevelName<WARNING1>() ;
  streamlog::out.addLevelName<WARNING2>() ;
  streamlog::out.addLevelName<WARNING3>() ;
  streamlog::out.addLevelName<WARNING4>() ;
  streamlog::out.addLevelName<ERROR>() ;
  streamlog::out.addLevelName<ERROR0>() ;
  streamlog::out.addLevelName<ERROR1>() ;
  streamlog::out.addLevelName<ERROR2>() ;
  streamlog::out.addLevelName<ERROR3>() ;
  streamlog::out.addLevelName<ERROR4>() ;
  streamlog::out.addLevelName<SILENT>() ;


  //-------- init logging level ------------
  std::string verbosity = Global::parameters->getStringVal("Verbosity" ) ;
  streamlog::logscope scope( streamlog::out ) ;

  scope.setLevel( verbosity ) ;


  //   std::map<std::string, int> verbosity_levels;
  //   verbosity_levels[std::string("VERBOSE")]=Processor::VERBOSE;
  //   verbosity_levels[std::string("DEBUG")]=Processor::DEBUG;
  //   verbosity_levels[std::string("MESSAGE")]=Processor::MESSAGE;
  //   verbosity_levels[std::string("WARNING")]=Processor::WARNING;
  //   verbosity_levels[std::string("ERROR")]=Processor::ERROR;
  //   verbosity_levels[std::string("SILENT")]=Processor::SILENT;

  //   std::string verbosity = Global::parameters->getStringVal("Verbosity" ) ;
  //   if( verbosity.size() > 0 ){
  // 	  Processor::Verbosity=verbosity_levels[verbosity];
  //   }

  createProcessors( *parser ) ;

#ifdef USE_GEAR

  std::string gearFile = Global::parameters->getStringVal("GearXMLFile" ) ;
  
  if( gearFile.size() > 0 ) {

    gear::GearXML gearXML( gearFile ) ;
    
    Global::GEAR = gearXML.createGearMgr() ;

    streamlog_out( MESSAGE )  << " ---- instantiated  GEAR from file  " << gearFile  << std::endl 
			      << *Global::GEAR << std::endl ;

  } else {

    streamlog_out( WARNING ) << " ---- no GEAR XML file given  --------- " << std::endl ;
    Global::GEAR = new gear::GearMgrImpl ;
  }

#endif

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
    
    // create lcio reader 
    LCReader* lcReader = LCFactory::getInstance()->createLCReader() ;
    
    
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
	
	std::cout << std::endl
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

	std::cout << std::endl
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
  
#ifdef USE_GEAR  

  if(  Global::GEAR != 0 ) 
    delete Global::GEAR ; 

#endif  
 
  return 0 ;
}

  
//   void  createProcessors(XMLParser&  parser) {
void  createProcessors( const IParser&  parser) {

  StringVec activeProcessors ;
  Global::parameters->getStringVals("ActiveProcessors" , activeProcessors ) ;

  StringVec procConds ;
  Global::parameters->getStringVals("ProcessorConditions" , procConds ) ;
    
  bool useConditions = ( activeProcessors.size() == procConds.size() ) ;

  //     for( StringVec::iterator m = activeProcessors.begin() ; m != activeProcessors.end() ; m++){
  for(unsigned int i=0 ; i<  activeProcessors.size() ; i++ ) {
      
    StringParameters* p = parser.getParameters( activeProcessors[i] )  ;

    if( p!=0 ){
      std::string type = p->getStringVal("ProcessorType") ;
	
      if( useConditions ) 
	ProcessorMgr::instance()->addActiveProcessor( type , activeProcessors[i] , p , procConds[i] )  ;
      else
	ProcessorMgr::instance()->addActiveProcessor( type , activeProcessors[i] , p )  ;

    } else{

      streamlog_out( WARNING )  << " Undefined processor : " << activeProcessors[i] << std::endl ;	
    }
  }
}

void  createProcessors(Parser&  parser) {
    
  StringVec activeProcessors ;
  Global::parameters->getStringVals("ActiveProcessors" , activeProcessors ) ;

  for( StringVec::iterator m = activeProcessors.begin() ; m != activeProcessors.end() ; m++){

    StringParameters* p = parser.getParameters( *m )  ;
    

    streamlog_out( MESSAGE ) << " Parameters for processor " << *m 
			     << std::endl 
			     << *p ; 

    if( p!=0 ){
      std::string type = p->getStringVal("ProcessorType") ;
      
      if( ProcessorMgr::instance()->addActiveProcessor( type , *m , p )  ){

	// 	Processor* processor =  ProcessorMgr::instance()->getActiveProcessor( *m ) ;
	//	processor->setParameters( p ) ;
      }
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
	    << "   Marlin -h                  \t print this help information" << std::endl 
	    << "   Marlin -?                  \t print this help information" << std::endl 
	    << "   Marlin -x                  \t print an example steering file to stdout" << std::endl 
	    << "   Marlin -c steer.xml        \t check the given steering file for consistency" << std::endl 
	    << "   Marlin -f old.xml new.xml  \t convert old xml files to new xml files for consistency check" 
	    << std::endl 
	    << "   Marlin -o old.steer new.xml\t convert old steering file to xml steering file" << std::endl 
	    << "   Marlin -l                  \t [deprecated: old format steering file example]" << std::endl 
	    << "   Marlin -d steer.xml flow.dot\t create a program flow diagram (see: http://www.graphviz.org)" << std::endl 
	    << std::endl 
	    << " Example: " << std::endl 
	    << " To create a new default steering file from any Marlin application, run" << std::endl 
	    << "     Marlin -x > mysteer.xml" << std::endl 
	    << " and then use either an editor or the MarlinGUI to modify the created steering file " << std::endl 
	    << " to configure your application and then run it. e.g. : " << std::endl 
	    << "     Marlin mysteer.xml > marlin.out 2>&1 &" << std::endl
	    << std::endl ;

  return(0) ;

}

