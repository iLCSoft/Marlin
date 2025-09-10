#include "marlin/TestProcessor.h"
#include "marlin/Exceptions.h"
#include <iostream>
#include <sstream>

#include "marlin/VerbosityLevels.h"


#include "streamlog/streamlog.h"

using namespace lcio ;

namespace marlin{



  TestProcessor aTestProcessor ;


  TestProcessor::TestProcessor() : Processor("TestProcessor"),
    _nRun (-1), 
    _nEvt(-1),
    _doCalibration(false), 
    _nLoops(-1) {

    _description = "Simple processor to test the marlin application."
      " Prints run and event number." ;
  }

  void TestProcessor::init() { 

    streamlog_out( MESSAGE ) << "TestProcessor::init()  " << name() 
			     << std::endl 
			     << "  parameters: " << std::endl 
			     << *parameters()  
			     << std::endl ;
    

#ifdef MARLIN_VERSION_GE
#if MARLIN_VERSION_GE( 0, 9, 8 )    
    streamlog_out( DEBUG ) << " marlin version is g.e. 0.9.8 " << std::endl ;
#endif
#endif

    _nRun = 0 ;
    _nEvt = 0 ;
    _doCalibration = false ;
    _nLoops = 0 ;
 

  }

  void TestProcessor::processRunHeader( LCRunHeader* run) { 


   streamlog_out( MESSAGE ) << " processRun() " 
			    << run->getRunNumber() 		  
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

      streamlog_out( DEBUG ) << " initialize  _doCalibration  and  _nLoops in first event :  " << evt->getEventNumber() 
			     << " run " << evt->getRunNumber() << std::endl ; 
    }
  
    setReturnValue( "Calibrating" , false ) ;

    if( _doCalibration ) {

      setReturnValue( "Calibrating" , true ) ;

      //      message<MESSAGE>(  log()
      streamlog_out( MESSAGE) << "processEvent()  ---CALIBRATING ------ "  
			      << " in event " << evt->getEventNumber() << " (run " 
			      << evt->getRunNumber() << ") " 
			      << std::endl ;
      
// 	) ;


      // your calibration goes here ....
    

    
      // --------------- 
      if( _nEvt == 3 ){  
      
	_nEvt = 0 ;
	++_nLoops ;
      
	if( _nLoops == 3 ){
	
	  _doCalibration = false ;
	
	}else{
	
	  throw RewindDataFilesException( this ) ;
	}
      }
    }
  
    //---------end example code  ----------------------------------------

    streamlog_out(MESSAGE) << " processing event " << evt->getEventNumber() 
			   << "  in run "          << evt->getRunNumber() 
			   << std::endl ;
    
    
//     streamlog_out(DEBUG)   << "(DEBUG)   local verbosity level: " << logLevelName() << std::endl ;
    streamlog_out(MESSAGE) << "(MESSAGE) local verbosity level: " << logLevelName() << std::endl ;
//     streamlog_out(WARNING) << "(WARNING) local verbosity level: " << logLevelName() << std::endl ;
//     streamlog_out(ERROR)   << "(ERROR)   local verbosity level: " << logLevelName() << std::endl ;




    // always return true  for ProcessorName
    setReturnValue( true ) ;
    
    // set ProcessorName.EvenNumberOfEvents == true if this processor has been called 2n (n=0,1,2,...) times 
    setReturnValue("EvenNumberOfEvents",  !( _nEvt % 2 )   ) ;

    
  }
  
  void TestProcessor::check( LCEvent * evt ) { 
    
    streamlog_out(DEBUG) << " check() "  // << dummy_method() 
			 << evt->getEventNumber() 
			 << " (run " << evt->getRunNumber() << ") "
			 << std::endl ;
    
    
  }
  
  void TestProcessor::end(){ 
    
    printEndMessage() ;

  }

  void TestProcessor::printEndMessage() const {

    streamlog_out(MESSAGE) << " end() "  
			   << " processed "     << _nEvt << " events in " 
			   << _nRun << " runs " << std::endl 
			   << std::endl ;
    
    
    // test deprecated method message 
    message<DEBUG>( " and this is really the final DEBUG message ....") ;
  }
  
}// namespace marlin
