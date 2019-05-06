#ifndef MARLIN_MARLINAPPLICATION_h
#define MARLIN_MARLINAPPLICATION_h 1

// -- marlin headers
#include <marlin/Application.h>

namespace marlin {
  
  class IScheduler ;
  class DataSourcePlugin ;

  /**
   *  @brief  MarlinApplication class
   *  Implementation of standard Marlin processing (single threaded program).
   *  Parse a steering file from which a processor chain is described, configure
   *  processor and run the processor chain using LCIO input files, read one by one.
   */
  class MarlinApplication : public Application {
  public:
    using Scheduler = std::shared_ptr<IScheduler> ;
    using EventList = std::vector<std::shared_ptr<EVENT::LCEvent>> ;
    using DataSource = std::shared_ptr<DataSourcePlugin> ;
    
  public:
    MarlinApplication() = default ;
    
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
    
  private:
    /// The event processing scheduler instance
    Scheduler                         _scheduler {nullptr} ;
    ///< The data source plugin
    DataSource                        _dataSource {nullptr} ;
  };

} // end namespace marlin

#endif
