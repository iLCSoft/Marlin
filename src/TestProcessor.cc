#include "marlin/TestProcessor.h"
#include "marlin/Exceptions.h"
#include <iostream>
#include <sstream>

#include "marlin/VerbosityLevels.h"



namespace marlin{



  TestProcessor aTestProcessor ;


  TestProcessor::TestProcessor() : Processor("TestProcessor") {

    _description = "Simple processor to test the marlin application."
      " Prints run and event number." ;
  }

  void TestProcessor::init() { 

    m_out( MESSAGE ) << "TestProcessor::init()  " << name() 
		     << std::endl 
		     << "  parameters: " << std::endl 
		     << *parameters()  
		     << m_endl ;
    

    _nRun = 0 ;
    _nEvt = 0 ;
  

  }

  void TestProcessor::processRunHeader( LCRunHeader* run) { 


    m_out( MESSAGE ) << " processRun() " 
		     << run->getRunNumber() 		  
		     << m_endl ;
    
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

      message<MESSAGE>(  log()
			 << "processEvent()  ---CALIBRATING ------ "  
			 << " in event " << evt->getEventNumber() << " (run " 
			 << evt->getRunNumber() << ") "

			 ) ;


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

    m_out(MESSAGE) << " processing event " << evt->getEventNumber() 
		   << "  in run "          << evt->getRunNumber() 
		   << m_endl ;
    
    
    // always return true  for ProcessorName
    setReturnValue( true ) ;
    
    // set ProcessorName.EvenNumberOfEvents == true if this processor has been called 2n (n=0,1,2,...) times 
    if( !( _nEvt % 2 )  )  
      setReturnValue("EvenNumberOfEvents", true ) ;
    
  }
  
  void TestProcessor::check( LCEvent * evt ) { 
    
    m_out(DEBUG) << " check() "  // << dummy_method() 
		 << evt->getEventNumber() 
		 << " (run " << evt->getRunNumber() << ") "
		 << m_endl ;
    
    
  }
  
  void TestProcessor::end(){ 
    
    printEndMessage() ;

  }

  void TestProcessor::printEndMessage() const {

    m_out(MESSAGE) << " end() "  
		   << " processed " << _nEvt << " events in " 
		   << _nRun << " runs " << std::endl 
		   << m_endl ;
    

    message<DEBUG>( " and this is really the final DEBUG message ....") ;
  }
  
}// namespace marlin
