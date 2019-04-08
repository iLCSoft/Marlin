#include "TestEventModifier.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/Logging.h"

#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

using namespace lcio ;
using namespace marlin ;


TestEventModifier aTestEventModifier ;


TestEventModifier::TestEventModifier() : Processor("TestEventModifier") {
  
  // modify processor description
  _description = "TestEventModifier make changes to the run header and the event data for testing" ;
  _nRun = -1;
  _nEvt = -1;
}


void TestEventModifier::init() { 

  _nRun = 0 ;
  _nEvt = 0 ;
  
}

void TestEventModifier::modifyRunHeader(EVENT::LCRunHeader * rh) {  
  
  // modify the run number
  
  LCRunHeaderImpl* rHdr = dynamic_cast< LCRunHeaderImpl* >( rh ) ;
  
  if( rHdr != 0 ) {
    
    rHdr->setRunNumber( _nRun + 42 ) ;
  }
  
}

void  TestEventModifier::modifyEvent(EVENT::LCEvent* e) {
  
  LCEventImpl* evt = dynamic_cast<LCEventImpl*>( e) ;

  if( evt != 0 ){

    evt->setEventNumber(  _nEvt + 42 ) ;
  }


}


void TestEventModifier::processRunHeader( LCRunHeader* run) { 

  if( run->getRunNumber() == _nRun + 42 ){

    streamlog_out(MESSAGE4)  << " processRunHeader modified run number to be : " 
			    << _nRun << " + 42 = " <<  run->getRunNumber() << std::endl ;

    _nRun++ ;


  } else {

    streamlog_out(ERROR)  << " processRunHeader LCRunHeader::runNumber not modified ! "  << std::endl; 


  }
  

} 

void TestEventModifier::processEvent( LCEvent * evt ) { 

    

  if( evt->getEventNumber() == _nEvt + 42 ){
    
    streamlog_out(MESSAGE4)  << " processEvent modified event number to be : " 
			    << _nEvt << " + 42 = " <<  evt->getEventNumber() << std::endl ;


    _nEvt ++ ;

  } else {
    
    streamlog_out(ERROR)  << " processEvent LCEvent::eventNumber not modified ! "  << std::endl; 
    
  }


  //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
		       << "   in run:  " << evt->getRunNumber() 
		       << std::endl ;
  


}



void TestEventModifier::check( LCEvent * ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void TestEventModifier::end(){ 
  
  streamlog_out(MESSAGE4) << name() 
			  << " modified " << _nEvt << " events in " << _nRun << " run "
			  << std::endl ;
  
}

