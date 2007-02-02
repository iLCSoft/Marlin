#include "marlin/TestProcessor.h"
#include "marlin/Exceptions.h"
#include <iostream>

namespace marlin{

TestProcessor aTestProcessor ;


TestProcessor::TestProcessor() : Processor("TestProcessor") {

  _description = "Simple processor to test the marlin application. Prints run and event number." ;
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


  _nEvt ++ ;

  //------------ example code that tests/demonstrates ------------
  //------------ the usage of Exceptions to steer program flow ------------
  //   
  //  --- skip every 7th event --------------

  //if( !(  _nEvt % 7 )  )
  //      throw SkipEventException( this ) ;
  
  //  ---  stop afteer processing 80 events -----

  //     if( _nEvt == 80   )
  //      throw StopProcessingException( this ) ;

  // --- loop 3 times over the first 3 events and do a 'calibration' ) ------

  if( isFirstEvent() ){
    _doCalibration = true ;
    _nLoops = 0 ;
  }
  
  if( _doCalibration ) {

    std::cout << "TestProcessor::processEvent()  ---CALIBRATING ------ " << name() 
	      << " in event " << evt->getEventNumber() << " (run " << evt->getRunNumber() << ") "	      << std::endl ;

    // your calibration goes here ....





    // --------------- 
    if( _nEvt == 3 ){  
      
      _nEvt = 0 ;
      ++_nLoops ;
      
      if( _nLoops == 3 ){
	
	_doCalibration = false ;
	setReturnValue( "Calibration" , false ) ;
	
      }else{
	
	setReturnValue( "Calibration" , true ) ;
	throw RewindDataFilesException( this ) ;
      }
    }
  }

  //---------end example code  ----------------------------------------
  




  std::cout << "TestProcessor::processEvent()  " << name() 
	    << " in event " << evt->getEventNumber() << " (run " << evt->getRunNumber() << ") "
	    << std::endl ;


  // always return true  for ProcessorName
  setReturnValue( true ) ;
  
  // set ProcessorName.EvenNumberOfEvents == true if this processor has been called 2n (n=0,1,2,...) times 
  if( !( _nEvt % 2 )  )  
    setReturnValue("EvenNumberOfEvents", true ) ;
  
}

void TestProcessor::check( LCEvent * evt ) { 
  std::cout << "TestProcessor::check()  " << name() 
	    << " in event " << evt->getEventNumber() << " (run " << evt->getRunNumber() << ") "
	    << std::endl ;
}

void TestProcessor::end(){ 
  std::cout << "TestProcessor::end()  " << name() 
	    << " processed " << _nEvt << " events in " << _nRun << " runs "
	    << std::endl ;
  
}

}// namespace marlin
