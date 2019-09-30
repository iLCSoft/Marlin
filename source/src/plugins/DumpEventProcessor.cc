
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/PluginManager.h>

// -- lcio headers
#include <UTIL/LCTOOLS.h>

namespace marlin {
  
  /** DumpEventProcessor simply dumps an event in the console
   *  <h4>Input - Prerequisites</h4>
   *  No input needed for this processor.
   *
   *  <h4>Output</h4>
   *  none
   *
   * @param DumpDetailed  make a detailed dump of the event
   *
   * @author R. Ete, DESY
   */
  class DumpEventProcessor : public Processor {
  public:
  	/**
     *  @brief Constructor
     */
  	DumpEventProcessor() ;

    // from Processor
  	void init() ;
  	void processEvent( EVENT::LCEvent * evt ) ;  	
  	
  protected:
    OptionalProperty<bool> _dumpDetailed {this, "DumpDetailed",
             "Whether to make a detailed dump of the event", true } ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  DumpEventProcessor::DumpEventProcessor() : 
    Processor("DumpEvent") {
  	// modify processor description
  	_description = "Simple processor to dump an event" ;             
    // duplicate and don't lock. Anyway, this processor in MT mode doesn't make sense...    
    forceRuntimeOption( Processor::RuntimeOption::Critical, false ) ;
    forceRuntimeOption( Processor::RuntimeOption::Clone, true ) ;
  }
  
  //--------------------------------------------------------------------------

  void DumpEventProcessor::init() {
  	// Print the initial parameters
  	printParameters() ;
  }
  
  //--------------------------------------------------------------------------

  void DumpEventProcessor::processEvent( EVENT::LCEvent * evt ) {
	   if( _dumpDetailed ) {
       UTIL::LCTOOLS::dumpEventDetailed( evt ) ;
     }
     else {
       UTIL::LCTOOLS::dumpEvent( evt ) ;
     }
  }
  
  // plugin declaration
  MARLIN_DECLARE_PROCESSOR( DumpEventProcessor ) 
}




