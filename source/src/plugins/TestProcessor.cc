
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/PluginManager.h>
#include <marlin/Exceptions.h>
#include <marlin/Logging.h>
#include <marlin/EventExtensions.h>

// -- std headers
#include <iostream>
#include <sstream>
#include <atomic>

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

    int                   _nRun {0} ;
    std::atomic_int       _nEvt {0} ;
    bool                  _doCalibration {false} ;
  } ;
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  TestProcessor::TestProcessor() : 
    Processor("TestProcessor") {
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
  }
  
  //--------------------------------------------------------------------------

  void TestProcessor::processRunHeader( EVENT::LCRunHeader* run) { 
   log<MESSAGE>() << " processRun() " 
			    << run->getRunNumber() 		  
			    << std::endl ;
    _nRun++ ;
  }
  
  //--------------------------------------------------------------------------
  
  void TestProcessor::processEvent( EVENT::LCEvent * evt ) {
    // event counter: add +1 and get the old value.
    // For example, if _nEvt is 0, it adds +1 and returns 0
    auto eventCounter = _nEvt.fetch_add(1) ;
    // This how we know if we are currently processing the first event
    const bool firstEvent = evt->runtime().ext<IsFirstEvent>() ;
    // Get the processor runtime conditions (not conditions data)
    // You can use them to set boolean values that next processors 
    // in the chain can use as conditions
    auto conditions = evt->runtime().ext<ProcessorConditions>() ;
    // An example usage in this processor ...
    const bool calibrate = (eventCounter % 3 ) == 0 ;
    
    if( firstEvent ) {
      log<DEBUG>() << " This is the first event :  " << evt->getEventNumber() 
			     << " run " << evt->getRunNumber() << std::endl ; 
    }
  
    conditions->set( this, "Calibrating" , calibrate ) ;

    if( calibrate ) {
      log<MESSAGE>() << "processEvent()  ---CALIBRATING ------ "  
			      << " in event " << evt->getEventNumber() << " (run " 
			      << evt->getRunNumber() << ") " 
			      << std::endl ;
    }

    log<MESSAGE>() << " processing event " << evt->getEventNumber() 
			   << "  in run "          << evt->getRunNumber() 
			   << std::endl ;
    
    log<MESSAGE>() << "(MESSAGE) local verbosity level: " << logLevelName() << std::endl ;

    // always return true for this processor
    conditions->set( this , true ) ;
    // set ProcessorName.EvenNumberOfEvents == true if this processor has been called 2n (n=0,1,2,...) times 
    conditions->set( this , "EvenNumberOfEvents", !( eventCounter % 2 ) ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void TestProcessor::check( EVENT::LCEvent * evt ) { 
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
  MARLIN_DECLARE_PROCESSOR( TestProcessor )
}
