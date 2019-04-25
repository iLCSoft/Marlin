#ifndef MARLIN_MARLINPEPAPPLICATION_h
#define MARLIN_MARLINPEPAPPLICATION_h 1

// -- marlin headers
#include <marlin/Application.h>
#include <marlin/ReaderListener.h>

// -- lcio headers
#include <MT/LCReader.h>

namespace marlin {

  class IScheduler ;

  /**
   *  @brief  MarlinPEPApplication class
   *
   *  Implementation of parallel Marlin processing (multi threaded program).
   *  Parse a steering file from which a processor chain is described, configure
   *  processor and run N processor chains in N threads. The number of threads
   *  is either automatically by the OS, by the number of input files or set
   *  manually in the steering file or command arguments (-j N). Events are read
   *  from the input file(s) and pushed in the thread pool to be distributed
   *  among the worker threads. LCRunHeader objects are processed synchronously, waiting
   *  for all current events being processed to be finished before calling processRunHeader()
   */
  class MarlinPEPApplication : public Application {
  public:
    using Scheduler = std::shared_ptr<IScheduler> ;
    using FileReader = MT::LCReader ;
    using FileList = EVENT::StringVec ;
    using EventList = std::vector<std::shared_ptr<EVENT::LCEvent>> ;

  public:
    MarlinPEPApplication() = default ;

  private:
    void runApplication() override ;
    void init() override ;
    void end() override ;
    void printUsage() const override ;

    void configureScheduler() ;
    void configureFileReader() ;

    void onEventRead( std::shared_ptr<EVENT::LCEvent> event ) ;
    void onRunHeaderRead( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    void processFinishedEvents( const EventList &events ) const ;

  private:
    ///< The event processing scheduler instance
    Scheduler                    _scheduler {nullptr} ;
    ///< The event reader listener
    ReaderListener               _readerListener {} ;
    ///< The lcio file reader
    FileReader                   _fileReader {FileReader::directAccess} ;
    ///< The LCIO input file list
    FileList                     _lcioFileList {} ;
    ///< The maximum number of record to read fron file
    int                          _maxRecordNumber {0} ;
    ///< The number of events to skip from begining of file
    int                          _skipNEvents {0} ;
  };

} // end namespace marlin

#endif
