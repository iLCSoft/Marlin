
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/PluginManager.h>

// -- lcio headers
#include <lcio.h>

// -- std headers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#else 
#include <sys/sysinfo.h>
#endif

namespace marlin {
  
  /** MemoryMonitorProcessor is a memory monitoring application for Marlin
   *  <h4>Input - Prerequisites</h4>
   *  No input needed for this processor.
   *
   *  <h4>Output</h4>
   *  none
   *
   * @param howOften  prints memory consumption every 'howOften' events
   *
   * @author N. Nikiforou, CERN,
   */
  class MemoryMonitorProcessor : public Processor {
  public:
  	/**
     *  @brief Constructor
     */
  	MemoryMonitorProcessor() ;

    // from Processor
  	void init() ;
  	void processEvent( EVENT::LCEvent * evt ) ;  	
  	
  protected:
    Property<int> _howOften {this, "howOften",
              "Print event number every N events", 1 } ;

    ///< Event counter
    unsigned int      _eventNumber {0} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  MemoryMonitorProcessor::MemoryMonitorProcessor() : 
    Processor("MemoryMonitor") {
  	// modify processor description
  	_description = "Simple processor to print out the memory consumption at defined intervals" ;
    // It doesn't make sense to create clones of this processor in MT environement
    // So we don't duplicate it per thread. To make it thread-safe we thus need to lock 
    // make it thread-safe by using a lock
    forceRuntimeOption( Processor::RuntimeOption::Critical, true ) ;
    // don't duplicate opening/writing of output file
    forceRuntimeOption( Processor::RuntimeOption::Clone, false ) ;
  }
  
  //--------------------------------------------------------------------------

  void MemoryMonitorProcessor::init() {
  	// Print the initial parameters
  	printParameters() ;
  }
  
  //--------------------------------------------------------------------------

  void MemoryMonitorProcessor::processEvent( EVENT::LCEvent * /*evt*/ ) {
    if (_eventNumber % _howOften == 0) {

#ifdef __APPLE__
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    
    if (KERN_SUCCESS != task_info(mach_task_self(),
      TASK_BASIC_INFO, (task_info_t)&t_info, 
                                  &t_info_count)) {
      log<ERROR>() << "Problem accessing system information from  MacOS X!"<< std::endl ;
    }
    log<MESSAGE>() << " Processed event  " << _eventNumber
    << " Resident size is: "<< t_info.resident_size<<" virtual size: "<<t_info.virtual_size
    << std::endl ;
#else
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    unsigned long physMemUsed = memInfo.totalram - memInfo.freeram;
    log<MESSAGE>() << " Processed event  " << _eventNumber
                          << " Physical memory in use: " << physMemUsed
                          << std::endl ;
#endif
    }
  	_eventNumber++ ;  	
  }
  
  // plugin declaration
  MARLIN_DECLARE_PROCESSOR( MemoryMonitorProcessor )
}




