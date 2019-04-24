
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/Logging.h>

// -- lcio headers
#include <lcio.h>

// -- std headers
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

namespace marlin {
  
    /** Simple processor for writing out a status message every n-th event.
     *
     *  <h4>Input - Prerequisites</h4>
     *  none
     *  <h4>Output</h4> 
     *  none
     * @parameter HowOften  print run and event number for every HowOften-th event
     * 
     * @author A.Sailer CERN
     * @version $Id:$
     */
  class Statusmonitor : public Processor {
    
   public:     
    Statusmonitor() ;
    
    Processor *newProcessor() { return new Statusmonitor ; }

    void init() ;
    
    /** Called for every run.
     */
    void processRunHeader( EVENT::LCRunHeader* run ) ;
    
    /** Called for every event - the working horse.
     */
    void processEvent( EVENT::LCEvent * evt ) ; 
    
    /** Called after data processing for clean up.
     */
    void end() ;
    
  private:
    // processor parameters
    int                 _howOften {1} ;

    // runtime members
    int                 _nRun {0} ;
    std::atomic<int>    _nEvt {0} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  Statusmonitor::Statusmonitor() : Processor("Statusmonitor") {
    
    // modify processor description
    _description = "Statusmonitor prints out information on running Marlin Job: Prints number of runs run and current number of the event. Counting is sequential and not the run or event ID." ;

    registerProcessorParameter("HowOften",
  			     "Print the event number every N events",
  			     _howOften, 
  			     int(1) ) ;
             
   // no need to lock, this processor is thread safe
   forceRuntimeOption( Processor::RuntimeOption::Critical, false ) ;
   // don't duplicate since it is thread safe and hold no big data
   forceRuntimeOption( Processor::RuntimeOption::Clone, false ) ;
  }

  //--------------------------------------------------------------------------

  void Statusmonitor::init() { 
    log<DEBUG>() << "INIT CALLED  " << std::endl ;
    // usually a good idea to
    printParameters() ;
  }
  
  //--------------------------------------------------------------------------

  void Statusmonitor::processRunHeader( EVENT::LCRunHeader* ) { 
    _nRun++ ;
  }
  
  //--------------------------------------------------------------------------

  void Statusmonitor::processEvent( EVENT::LCEvent *  ) {
    auto eventid = _nEvt.fetch_add(1) ;
    if (eventid % _howOften == 0) {
      log<MESSAGE>() 
        << " ===== Run  : " << std::setw(7) << _nRun
        << "  Event: " << std::setw(7) << eventid << std::endl;
    }
  }
  
  //--------------------------------------------------------------------------

  void Statusmonitor::end() {
    log<MESSAGE>() << "Statusmonitor::end()  " << name()  << " processed " << _nEvt << " events in " << _nRun << " runs"	<< std::endl ;
  }
  
  // processor declaration
  Statusmonitor aStatusmonitor ;
  
}

