#include "marlin/LCIOOutputProcessor.h"
#include <iostream>

#include "IMPL/LCRunHeaderImpl.h"

namespace marlin{
 
LCIOOutputProcessor anLCIOOutputProcessor ;

LCIOOutputProcessor::LCIOOutputProcessor() : Processor("LCIOOutputProcessor") {

  _description = "Writes the current event to the specified LCIO outputfile."
    " Needs to be the last ActiveProcessor." ;
  
  
  registerProcessorParameter( "LCIOOutputFile" , 
			      " name of output file "  ,
			      _lcioOutputFile ,
			      std::string("outputfile.slcio") ) ;
  
  registerProcessorParameter( "LCIOWriteMode" , 
			      "write mode for output file:  WRITE_APPEND or WRITE_NEW"  ,
			      _lcioWriteMode ,
			      std::string("None") ) ;
  
}

void LCIOOutputProcessor::init() { 

  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;

  _lcWrt = LCFactory::getInstance()->createLCWriter() ;

  if( _lcioWriteMode == "WRITE_APPEND" ) {
    
    _lcWrt->open( _lcioOutputFile , LCIO::WRITE_APPEND ) ;
  }
  else if( _lcioWriteMode == "WRITE_NEW" ) {
    
    _lcWrt->open( _lcioOutputFile , LCIO::WRITE_NEW ) ;
  }
  else {
    _lcWrt->open( _lcioOutputFile ) ;
  }

//   _lcWrt->writeRunHeader( new LCRunHeaderImpl ) ;
//   _lcWrt->close() ;
//   _lcWrt->open( _lcioOutputFile , LCIO::WRITE_APPEND ) ;
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

  std::cout  << std::endl 
	     << "LCIOOutputProcessor::end()  " << name() 
	     << _nEvt << " events in " << _nRun << " runs written to file  " 
	     <<  _lcioOutputFile  
	     << std::endl
	     << std::endl ;
  
  _lcWrt->close() ;

}

} // namespace marlin{
