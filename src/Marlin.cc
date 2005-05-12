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
#include "IO/LCReader.h"

#include "marlin/Parser.h"

#include "marlin/Global.h"

#include <sstream>
#include <fstream>
#include <string>
#include <assert.h>


using namespace lcio ;
using namespace marlin ;


void createProcessors( Parser&  parser ) ;
void listAvailableProcessors() ;


/** LCIO framework that can be used to analyse LCIO data files
 *  in a modular way. All tasks have to be implemented in Subclasses
 *  of Processor. They will be called in the order specified in the steering file.
 *
 */
int main(int argc, char** argv ){
  
  const char* steeringFileName ;
  
  // read file name from command line
  if( argc > 1 ){

    if( std::string(argv[1]) == "-l" ){
      listAvailableProcessors() ;
      exit(0) ;
    }

    steeringFileName = argv[1] ;

  } else {
    std::cout << " usage: MyMarlinApp mypersonal.steer" << std::endl 
	 << std::endl ;
    exit(1) ;
  }
  
  
  Parser  parser( steeringFileName ) ;
  assert( ( Global::parameters = parser.getParameters("Global") ) != 0 ) ;

  std::cout <<  "---- Global parameters : " << std::endl 
	    << *Global::parameters  <<  std::endl ;

  createProcessors( parser ) ;
  

  StringVec lcioInputFiles ; 

//   assert( (Global::parameters->getStringVals("LCIOInputFiles" , lcioInputFiles ) ).size() > 0 ) ;

  if ( (Global::parameters->getStringVals("LCIOInputFiles" , lcioInputFiles ) ).size() == 0 ){

    int maxRecord = Global::parameters->getIntVal("MaxRecordNumber");
    ProcessorMgr::instance()->init() ; 
    // fixme: pass maxRecord-1 (because of the runheader, which is generated)?
    ProcessorMgr::instance()->readDataSource(maxRecord) ; 
    ProcessorMgr::instance()->end() ; 
    
  } else { 
    
    int maxRecord = Global::parameters->getIntVal("MaxRecordNumber") ;
    
    // create lcio reader 
    LCReader* lcReader = LCFactory::getInstance()->createLCReader() ;
    
    
    lcReader->registerLCRunListener( ProcessorMgr::instance() ) ; 
    lcReader->registerLCEventListener( ProcessorMgr::instance() ) ; 
    
    
    // process the data
    lcReader->open( lcioInputFiles  ) ; 
    
    ProcessorMgr::instance()->init() ; 
    
    if( maxRecord > 0 ){
      
      try{
	lcReader->readStream( maxRecord ) ;
      }
      catch( lcio::EndOfDataException& e){
	
	std::cout << "Warning: " << e.what() << std::endl ;
      }
      
    } else {
      lcReader->readStream() ;
    }
    
    ProcessorMgr::instance()->end() ; 
    lcReader->close() ;
    delete lcReader ;
    
  }
  
  return 0 ;
}

  
  void  createProcessors(Parser&  parser) {
    
  StringVec activeProcessors ;
  Global::parameters->getStringVals("ActiveProcessors" , activeProcessors ) ;

  for( StringVec::iterator m = activeProcessors.begin() ; m != activeProcessors.end() ; m++){

    StringParameters* p = parser.getParameters( *m )  ;
    

//     std::cout << " Parameters for processor " << *m 
// 	      << std::endl 
// 	      << *p ; ;

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
