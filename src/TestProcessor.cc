#include "marlin/TestProcessor.h"
#include <iostream>

namespace marlin{

TestProcessor aTestProcessor ;


TestProcessor::TestProcessor() : Processor("TestProcessor") {

}


void TestProcessor::init() { 

  std::cout << "TestProcessor::init()  " << name() 
	    << std::endl 
	    << "  parameters: " << std::endl 
	    << *parameters()  ;

  _nRun = 0 ;
  _nEvt = 0 ;
  
}

void TestProcessor::processRunHeader( LCRunHeader* run) { 
  std::cout << "TestProcessor::processRun()  " << name() 
	    << " in run " << run->getRunNumber() 
	    << std::endl ;

  _nRun++ ;
} 

void TestProcessor::processEvent( LCEvent * evt ) { 
  std::cout << "TestProcessor::processEvent()  " << name() 
	    << " in event " << evt->getEventNumber() << " (run " << evt->getRunNumber() << ") "
	    << std::endl ;
  _nEvt ++ ;
}

void TestProcessor::end(){ 
  std::cout << "TestProcessor::end()  " << name() 
	    << " processed " << _nEvt << " events in " << _nRun << " runs "
	    << std::endl ;
  
}

}// namespace marlin
