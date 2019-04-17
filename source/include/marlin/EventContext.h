#ifndef MARLIN_EVENTCONTEXT_h
#define MARLIN_EVENTCONTEXT_h 1

// -- marlin headers
#include <marlin/RandomSeedManager.h>
#include <marlin/LogicalExpressions.h>

// -- std headers
#include <memory>
#include <thread>
#include <string>

namespace EVENT {
  class LCEvent ;
}

namespace marlin {

  class Processor ;

  /**
   *  @brief  EventContext class
   */
  class EventContext {
  public:
    using RandomSeedMap = std::unique_ptr<RandomSeedManager::RandomSeedMap> ;
    using RandomSeedType = RandomSeedManager::SeedType ;
    using RuntimeConditions = std::shared_ptr<LogicalExpressions> ;
    using Event = std::shared_ptr<EVENT::LCEvent> ;

  public:
    EventContext() = default ;

    /**
     *  @brief  Get the associated event
     */
    Event event() const ;

    /**
     *  @brief  Get the random seed for a given processor
     *
     *  @param  processor the processor pointer
     */
    RandomSeedType randomSeed( const Processor *const processor ) const ;

    /**
     *  @brief  Set the runtime condition of the processor
     *
     *  @param  processor the processor pointer
     *  @param  value the runtime condition to set
     */
    void setRuntimeCondition( const Processor *const processor, bool value ) ;

    /**
     *  @brief  Set the named runtime condition of the processor
     *
     *  @param  processor the processor pointer
     *  @param  name the condition name
     *  @param  value the runtime condition to set
     */
    void setRuntimeCondition( const Processor *const processor, const std::string &name, bool value ) ;

    /**
     *  @brief  Check whether the runtime condition is true
     *
     *  @param  name the condition name
     */
    bool checkRuntimeCondition( const std::string &name ) const ;

    /**
     *  @brief  Get the event context thread id
     */
    std::thread::id threadID() const ;

    /**
     *  @brief  Set the current thread id
     */
    void setThreadID() ;

    /**
     *  @brief  Set the event
     *
     *  @param  evt the event
     */
    void setEvent( Event evt ) ;

    /**
     *  @brief  Set the random seeds
     *
     *  @param  seeds the random seed map
     */
    void setRandomSeeds( RandomSeedMap seeds ) ;

    /**
     *  @brief  Setup the initial runtime conditions
     *
     *  @param  conds the initial processors runtime conditions
     */
    void setupRuntimeConditions( const std::map<std::string, std::string> &conds ) ;

  private:
    /// The runtime conditions
    RuntimeConditions     _runtimeConditions {std::make_shared<LogicalExpressions>()} ;
    /// The random seed map for the current event
    RandomSeedMap         _randomSeeds {nullptr} ;
    /// The event attached to this context
    Event                 _event {nullptr} ;
    /// The thread id in which the event is currently running
    std::thread::id       _threadID {} ;
  };
}

#endif
