#ifndef MARLIN_WHITERBOARD_h
#define MARLIN_WHITERBOARD_h 1

// -- marlin headers
#include "marlin/LogicalExpressions.h"
#include "marlin/RandomSeedManager.h"

#include <mutex>

namespace EVENT {
  class LCEvent ;
}

namespace marlin {

  class EventContext ;
  class Application ;

  /**
   *  @brief  Whiteboard class
   *  Manage runtime operations at event level. The whiteboard
   *  stores event entries and associated runtime metadata needed
   *  for sequential and parallel event processing
   */
  class Whiteboard {
  public:
    using EventContextMap = std::map<std::shared_ptr<EVENT::LCEvent>, std::shared_ptr<EventContext>> ;
    using ConditionsMap = std::map<std::string, std::string> ;

  public:
    Whiteboard() = default ;

    void init( const Application &app ) ;

    std::shared_ptr<EventContext> createEventContext( std::shared_ptr<EVENT::LCEvent> event ) ;

    void removeEventContext( std::shared_ptr<EventContext> ctx ) ;

    std::shared_ptr<EventContext> retrieveEventContext( std::shared_ptr<EVENT::LCEvent> event ) const ;

  private:
    mutable std::mutex                      _mutex {} ;
    std::shared_ptr<RandomSeedManager>      _rndSeedMgr {} ;
    EventContextMap                         _eventContexts {} ;
    ConditionsMap                           _initialConditions {} ;
  };

} // end namespace marlin

#endif
