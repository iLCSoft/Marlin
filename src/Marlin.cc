#include "lcio.h"
#include "LCIOSTLTypes.h"

#include "ProcessorMgr.h"
#include "Processor.h"
#include "IO/LCReader.h"

#include "Parser.h"

#include "LCGlobal.h"

#include <sstream>
#include <fstream>
#include <string>
#include <assert.h>

using namespace std ;
using namespace lcio ;



void createProcessors( Parser&  parser ) ;

/** LCIO framework that can be used to ananlyse LCIO data files
 *  in a modular way. All tasks have to be implemented in Subclasses
 *  of Processor. They will be called in the order specified in the steering file.
 *
 */
int main(int argc, char** argv ){
  
  const char* steeringFileName ;
  
  // read file name from command line
  if( argc > 1 ){
    steeringFileName = argv[1] ;
  } else {
    cout << " usage: Marlin lcioframe.steer" << endl 
	 << " steering file name "
	 << endl ;
    exit(1) ;
  }
  
  
  Parser  parser( steeringFileName ) ;
  assert( ( LCGlobal::parameters = parser.getParameters("Global") ) != 0 ) ;

  std::cout <<  "Global parameters : " << std::endl 
	    << *LCGlobal::parameters  <<  std::endl ;

  createProcessors( parser ) ;
  

  StringVec lcioInputFiles ; 

  assert( (LCGlobal::parameters->getStringVals("LCIOInputFiles" , lcioInputFiles ) ).size() > 0 ) ;

  // create lcio reader 
  LCReader* lcReader = LCFactory::getInstance()->createLCReader() ;
  

  lcReader->registerLCRunListener( ProcessorMgr::instance() ) ; 
  lcReader->registerLCEventListener( ProcessorMgr::instance() ) ; 


  // process the data
  lcReader->open( lcioInputFiles[0]  ) ; 
  // FIXME - read list of files
  //  lcReader->open( lcioInputFiles  ) ; 

  ProcessorMgr::instance()->init() ; 
  lcReader->readStream() ;
  ProcessorMgr::instance()->end() ; 
  lcReader->close() ;

  return 0 ;
}







void  createProcessors(Parser&  parser) {
  
  StringVec activeProcessors ;
  LCGlobal::parameters->getStringVals("ActiveProcessors" , activeProcessors ) ;

  for( StringVec::iterator m = activeProcessors.begin() ; m != activeProcessors.end() ; m++){

    StringParameters* p = parser.getParameters( *m )  ;
    

//     std::cout << " Parameters for processor " << *m 
// 	      << std::endl 
// 	      << *p ; ;

    if( p!=0 ){
      std::string type = p->getStringVal("ProcessorType") ;
      
      if( ProcessorMgr::instance()->addActiveProcessor( type , *m )  ){

	Processor* processor =  ProcessorMgr::instance()->getActiveProcessor( *m ) ;

	processor->setParameters( p ) ;
      }
    }

  }
}




