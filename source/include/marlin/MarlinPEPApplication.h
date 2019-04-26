#ifndef MARLIN_MARLINPEPAPPLICATION_h
#define MARLIN_MARLINPEPAPPLICATION_h 1

// -- marlin headers
#include <marlin/Application.h>
#include <marlin/ReaderListener.h>

// -- lcio headers
#include <MT/LCReader.h>

namespace marlin {

  class IScheduler ;
  class DataSourcePlugin ;

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
    using DataSource = std::shared_ptr<DataSourcePlugin> ;

  public:
    MarlinPEPApplication() = default ;

  private:
    // from Application
    void runApplication() override ;
    void init() override ;
    void end() override ;
    void printUsage() const override ;

    /**
     *  @brief  Configure the scheduler
     */
    void configureScheduler() ;
    
    /**
     *  @brief  Configure the data source
     */
    void configureDataSource() ;

    /**
     *  @brief  Callback function to process an event received from the data source
     * 
     *  @param  event the event to process
     */
    void onEventRead( std::shared_ptr<EVENT::LCEvent> event ) ;
    
    /**
     *  @brief  Callback function to process a run header received from the data source
     * 
     *  @param  rhdr the run header to process
     */
    void onRunHeaderRead( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

    /**
     *  @brief  Processed finished events from the output queue
     *
     *  @param  events the list of finished events
     */
    void processFinishedEvents( const EventList &events ) const ;

  private:
    ///< The event processing scheduler instance
    Scheduler                    _scheduler {nullptr} ;
    ///< The data source plugin
    DataSource                   _dataSource {nullptr} ;
  };

} // end namespace marlin

#endif
