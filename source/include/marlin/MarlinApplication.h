#ifndef MARLIN_MARLINAPPLICATION_h
#define MARLIN_MARLINAPPLICATION_h 1

// -- marlin headers
#include "marlin/Application.h"
#include "marlin/Scheduler.h"

// -- lcio headers
#include "LCIOSTLTypes.h"

namespace IO {
  class LCReader ;
}

namespace marlin {

  /**
   *  @brief  MarlinApplication class
   *  Implementation of standard Marlin processing (single threaded program).
   *  Parse a steering file from which a processor chain is described, configure
   *  processor and run the processor chain using LCIO input files, read one by one.
   */
  class MarlinApplication : public Application {
  public:
    MarlinApplication() = default ;
    
  private:
    void runApplication() ;
    void init() ;
    void end() {}
    void printUsage() const ;
    
  private:
    /// The processor scheduler
    Scheduler                         _scheduler {} ;
    /// The LCReader instance
    std::shared_ptr<IO::LCReader>     _lcReader {nullptr} ;
    /// The LCIO inout files to read and process
    EVENT::StringVec                  _lcioInputFiles {} ;
    /// The maximum number of record to read from files (events and run headers)
    int                               _maxRecord {0} ;
    /// The number of events to skip on file opening
    int                               _skipNEvents {0} ;
  };

} // end namespace marlin

#endif
