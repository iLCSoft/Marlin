#ifndef MARLIN_SCHEDULER_h
#define MARLIN_SCHEDULER_h 1

// -- std headers
#include <vector>
#include <memory>

// -- marlin headers
#include "marlin/LogicalExpressions.h"
#include "marlin/RandomSeedGenerator.h"
#include "marlin/Logging.h"

// lcio forward declaration
namespace EVENT {
  class LCEvent ;
  class LCRunHeader ;
}

namespace marlin {
  
  class Application ;
  class Processor ;
  
  /**
   *  @brief  Scheduler class
   *  Implementation of a single processor chain, run 
   *  sequentially within a single thread
   */
  class Scheduler {
    /**
     *  @brief  TimeMetadata struct
     *  Handle runtime information of processors
     */
    struct TimeMetadata {
      /// The total processing time of a processor (processEvent and modifyEvent)
      double   _processingTime {0.} ;
      /// The event counter
      int      _eventCounter {0} ;
    };
    
  public:
    // typedefs
    typedef std::vector<std::shared_ptr<Processor>>           ProcessorList ;
    typedef std::map<std::string, TimeMetadata>               TimeMap ;
    typedef std::map< const std::string , int >               SkippedEventMap ;
    typedef RandomSeedGenerator<Processor>                    SeedGeneratorType ;
    typedef Logging::Logger                                   Logger ;
    
  public:
    ~Scheduler() = default ;
    Scheduler(const Scheduler&) = delete ;
    Scheduler& operator=(const Scheduler&) = delete ;
    
    /**
     *  @brief  Constructor
     */
    Scheduler() ;

    /**
     *  @brief  Initialize the scheduler with the application
     * 
     *  @param  app the application in which the scheduler will run
     */
    void init( Application *app ) ;
    
    /**
     *  @brief  Process an event
     * 
     *  @param  event the event to process
     */
    void processEvent( EVENT::LCEvent *event ) ;
    
    /**
     *  @brief  Process the run header (start of run)
     * 
     *  @param  header the run header to process
     */
    void processRunHeader( EVENT::LCRunHeader *header ) ;
    
    /**
     *  @brief  Modify an event. Called before processEvent()
     * 
     *  @param  event the event to modify
     */
    void modifyEvent( EVENT::LCEvent *event ) ;
    
    /**
     *  @brief  Modify the run header (start of run). Called before processRunHeader()
     * 
     *  @param  header the run header to modify
     */
    void modifyRunHeader( EVENT::LCRunHeader *header ) ;

    /**
     *  @brief  Terminate and cleanup scheduler
     */
    void end() ;
    
    /**
     *  @brief  Get the runtime logical expressions
     */
    std::shared_ptr<LogicalExpressions> runtimeConditions() const ;

    /**
     *  @brief  Get the random seed generator
     */
    std::shared_ptr<SeedGeneratorType> randomSeedGenerator() const ;
    
    /**
     *  @brief  Get the scheduler logger instance
     */
    Logger logger() const ;
    
    /**
     *  @brief  Get the application in which the scheduler has been created
     */
    const Application &app() const ;
    
  private:
    /// The processor chain
    ProcessorList                       _processors {} ;
    /// Execution time measurement for each processor
    TimeMap                             _processorTimes {} ;
    /// Whether modifyEvent/modifyRunHeader should be called
    bool                                _allowModify {false} ;
    /// Whether to suppress call of check() method
    bool                                _suppressCheck {false} ;
    /// Skip event exception map
    SkippedEventMap                     _skipEventMap {} ;
    /// Runtime processor boolean conditions
    std::shared_ptr<LogicalExpressions> _runtimeConditions {} ;
    /// The processor event seeder mapping
    std::shared_ptr<SeedGeneratorType>  _rdmSeedGenerator {nullptr} ;
    /// The application in which the scheduler is running
    Application                        *_application {nullptr} ;
    /// The scheduler logger
    Logger                              _logger {nullptr} ;
  };

} // end namespace marlin

#endif
