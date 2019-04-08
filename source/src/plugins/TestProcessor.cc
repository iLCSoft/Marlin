
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/Logging.h>

// -- std headers
#include <iostream>
#include <sstream>

// -- lcio headers
#include <lcio.h>

namespace marlin {

  /** Simple processor for testing. 
   *  Writes something to stdout for every callbackmethod.
   * 
   *  <h4>Input - Prerequisites</h4>
   *  none
   *
   *  <h4>Output</h4> 
   *  none
   * 
   * @author F. Gaede, DESY
   * @version $Id: TestProcessor.h,v 1.7 2007-05-23 13:12:21 gaede Exp $ 
   */

  class TestProcessor : public Processor {
    
  public:
    
    Processor*  newProcessor() { return new TestProcessor ; }
    
    TestProcessor() ;
    
    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init() ;
    
    /** Called for every run.
     */
    void processRunHeader( EVENT::LCRunHeader* run ) ;
    
    /** Called for every event - the working horse.
     */
    void processEvent( EVENT::LCEvent * evt ) ; 
    
    
    void check( EVENT::LCEvent * evt ) ; 
    
    /** Called after data processing for clean up.
     */
    void end() ;
    
  protected:

    /**Test method for const.*/
    void printEndMessage() const ;

    int       _nRun {0} ;
    int       _nEvt {0} ;
    bool      _doCalibration {false} ;
    int       _nLoops {0} ;
  } ;
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  TestProcessor::TestProcessor() : Processor("TestProcessor"),
    _nRun (-1), 
    _nEvt(-1),
    _doCalibration(false), 
    _nLoops(-1) {

    _description = "Simple processor to test the marlin application."
      " Prints run and event number." ;
  }
  
  //--------------------------------------------------------------------------

  void TestProcessor::init() { 

    log<MESSAGE>() << "TestProcessor::init()  " << name() 
			     << std::endl 
			     << "  parameters: " << std::endl 
			     << *parameters()  
			     << std::endl ;
    

#ifdef MARLIN_VERSION_GE
#if MARLIN_VERSION_GE( 0, 9, 8 )    
    log<DEBUG>() << " marlin version is g.e. 0.9.8 " << std::endl ;
#endif
#endif

    _nRun = 0 ;
    _nEvt = 0 ;
    _doCalibration = false ;
    _nLoops = 0 ;
  }
  
  //--------------------------------------------------------------------------

  void TestProcessor::processRunHeader( LCRunHeader* run) { 
   log<MESSAGE>() << " processRun() " 
			    << run->getRunNumber() 		  
			    << std::endl ;
    
    _nRun++ ;
  }
  
  //--------------------------------------------------------------------------

  void TestProcessor::processEvent( LCEvent * evt ) { 
    _nEvt ++ ;
    //------------ example code that tests/demonstrates ------------
    //------------ the usage of Exceptions to steer program flow ------------
    // --- loop 3 times over the first 3 events and do a 'calibration' ) ------

    if( isFirstEvent() ){
      _doCalibration = true ;
      _nLoops = 0 ;

      log<DEBUG>() << " initialize  _doCalibration  and  _nLoops in first event :  " << evt->getEventNumber() 
			     << " run " << evt->getRunNumber() << std::endl ; 
    }
  
    setReturnValue( "Calibrating" , false ) ;

    if( _doCalibration ) {

      setReturnValue( "Calibrating" , true ) ;

      log<MESSAGE>() << "processEvent()  ---CALIBRATING ------ "  
			      << " in event " << evt->getEventNumber() << " (run " 
			      << evt->getRunNumber() << ") " 
			      << std::endl ;

      if( _nEvt == 3 ) {
      	_nEvt = 0 ;
      	++_nLoops ;
      	if( _nLoops == 3 ) {
      	  _doCalibration = false ;
      	}
        else {
      	  throw RewindDataFilesException( this ) ;
      	}
      }
    }

    log<MESSAGE>() << " processing event " << evt->getEventNumber() 
			   << "  in run "          << evt->getRunNumber() 
			   << std::endl ;
    
    log<MESSAGE>() << "(MESSAGE) local verbosity level: " << logLevelName() << std::endl ;

    // always return true  for ProcessorName
    setReturnValue( true ) ;
    
    // set ProcessorName.EvenNumberOfEvents == true if this processor has been called 2n (n=0,1,2,...) times 
    setReturnValue("EvenNumberOfEvents",  !( _nEvt % 2 )   ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void TestProcessor::check( LCEvent * evt ) { 
    log<DEBUG>() << " check() "
			 << evt->getEventNumber() 
			 << " (run " << evt->getRunNumber() << ") "
			 << std::endl ;
  }
  
  //--------------------------------------------------------------------------
  
  void TestProcessor::end(){ 
    printEndMessage() ;
  }
  
  //--------------------------------------------------------------------------

  void TestProcessor::printEndMessage() const {
    log<MESSAGE>() << " end() "  
			   << " processed "     << _nEvt << " events in " 
			   << _nRun << " runs " << std::endl 
			   << std::endl ;
  }
  
  // processor declaration
  TestProcessor aTestProcessor ;
  
}// namespace marlin
