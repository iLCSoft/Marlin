#ifndef MARLIN_MARLINMTAPPLICATION_h
#define MARLIN_MARLINMTAPPLICATION_h 1

// -- marlin headers
#include "marlin/Application.h"

namespace marlin {

  /**
   *  @brief  MarlinMTApplication class
   *  Implementation of parallel Marlin processing (multi threaded program).
   *  Parse a steering file from which a processor chain is described, configure
   *  processor and run N processor chains in N threads. The number of threads 
   *  is either automatically by the OS, by the number of input files or set 
   *  manually in the steering file or command arguments (-j N).
   *  Each thread is processing a full processor chain with its own LCReader
   */
  class MarlinMTApplication : public Application {
  public:
    MarlinMTApplication() = default ;
    
  private:
    void runApplication() ;
    void init() ;
    void end() {}
    void printUsage() const ;
  };

} // end namespace marlin

#endif
