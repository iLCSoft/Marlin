#include "LCIOOutputProcessor.h"
#include <iostream>

#include "IMPL/LCRunHeaderImpl.h"


LCIOOutputProcessor anLCIOOutputProcessor ;

LCIOOutputProcessor::LCIOOutputProcessor() : Processor("LCIOOutputProcessor") {

}

void LCIOOutputProcessor::init() { 

  std::cout << "LCIOOutputProcessor::init()  " << name() 
	    << std::endl 
	    << "  parameters: " << std::endl 
	    << *parameters()  ;
  

  _nRun = 0 ;
  _nEvt = 0 ;

  _lcWrt = LCFactory::getInstance()->createLCWriter() ;


  std::string fName = parameters()->getStringVal( LCIOOUTPUTFILE ) ;

  std::string mode =  parameters()->getStringVal( LCIOWRITEMODE )  ;
  
  if( mode == "WRITE_APPEND" ) {
    
    _lcWrt->open( fName , LCIO::WRITE_APPEND ) ;
  }
  else if( mode == "WRITE_NEW" ) {
    
    _lcWrt->open( fName , LCIO::WRITE_NEW ) ;
  }
  else {
    _lcWrt->open( fName ) ;
  }

//   _lcWrt->writeRunHeader( new LCRunHeaderImpl ) ;
//   _lcWrt->close() ;
//   _lcWrt->open( fName , LCIO::WRITE_APPEND ) ;
}



void LCIOOutputProcessor::processRunHeader( LCRunHeader* run) { 

//    std::cout << "LCIOOutputProcessor::processRun()  " << name() <<" this << " << this
// 	     << " in run " << run->getRunNumber() 
// 	     << std::endl ;

  _lcWrt->writeRunHeader( run ) ;

  _nRun++ ;
} 

void LCIOOutputProcessor::processEvent( LCEvent * evt ) { 
//   std::cout << "LCIOOutputProcessor::processEvent()  " << name() 
// 	    << " in event " << evt->getEventNumber() << " (run " << evt->getRunNumber() << ") "
// 	    << std::endl ;

  _lcWrt->writeEvent( evt ) ;

  _nEvt ++ ;
}

void LCIOOutputProcessor::end(){ 

  std::cout << "LCIOOutputProcessor::end()  " << name() 
	    << _nEvt << " events in " << _nRun << " runs written to file  " 
	    <<  parameters()->getStringVal( LCIOOUTPUTFILE ) 
	    << std::endl ;
  
  _lcWrt->close() ;

}

