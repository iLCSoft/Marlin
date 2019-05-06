#ifndef MARLIN_SIMPLESCHEDULER_h
#define MARLIN_SIMPLESCHEDULER_h 1

// -- marlin headers
#include <marlin/IScheduler.h>
#include <marlin/Logging.h>
#include <marlin/RandomSeedManager.h>

namespace marlin {
  
  class SuperSequence ;
  class Sequence ;

  /**
   *  @brief  SimpleScheduler class
   */
  class SimpleScheduler : public IScheduler {
  public:
    using ConditionsMap = std::map<std::string, std::string> ;
    using Logger = Logging::Logger ;
    using ProcessorSequence = std::shared_ptr<SuperSequence> ;
    
  public:
    SimpleScheduler() = default ;
    
    // from IScheduler interface
    void init( const Application *app ) ;
    void end() ;
    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;    
    void pushEvent( std::shared_ptr<EVENT::LCEvent> event ) ;
    void popFinishedEvents( std::vector<std::shared_ptr<EVENT::LCEvent>> &events ) ;
    std::size_t freeSlots() const ;
    
  private:
    ///< The logger instance
    Logger                           _logger {nullptr} ;
    ///< The processor super sequence
    ProcessorSequence                _superSequence {nullptr} ;
    ///< Initial processor runtime conditions from steering file
    ConditionsMap                    _conditions {} ;
    ///< The random seed manager
    RandomSeedManager                _rdmSeedMgr {} ;
    ///< Whether the currently pushed event is the first one
    bool                             _isFirstEvent {true} ;
    ///< The current event being processed
    std::shared_ptr<EVENT::LCEvent>  _currentEvent {nullptr} ;
  };

} // end namespace marlin

#endif
