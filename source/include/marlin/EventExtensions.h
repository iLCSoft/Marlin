#ifndef MARLIN_EVENTEXTENSIONS_h
#define MARLIN_EVENTEXTENSIONS_h 1

// -- marlin headers
#include <marlin/RandomSeedManager.h>
#include <marlin/LogicalExpressions.h>

// -- lcio headers
#include <LCRTRelations.h>

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
   *  @brief  EventExtension class
   *  Metadata attached to LCEvent by the Marlin framework for runtime processing purpose only.
   *  It can be accessed e.g from processors as :
   *  @code{cpp}
   *  void MyProcessor::processEvent( EVENT::LCEvent *event ) {
   *    auto extension = event->runtime().ext<RuntimeExtension>() ;
   *    // for example, get a random seed
   *    auto randomSeed = extension->randomSeed( this ) ;
   *    // do some random stuff
   *  }
   *  @endcode
   *  Note that the extension is owned by the event and must not be deleted by users!
   */
  class EventExtension {
  public:
    using RandomSeedMap = std::unique_ptr<RandomSeedManager::RandomSeedMap> ;
    using RandomSeedType = RandomSeedManager::SeedType ;
    using RuntimeConditions = std::shared_ptr<LogicalExpressions> ;
    
  public:
    EventExtension() = default ;
    ~EventExtension() = default ;
    EventExtension(const EventExtension&) = delete ;
    EventExtension& operator=(const EventExtension&) = delete ;
    
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
    
  private:
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
  };

  /**
   *  @brief  RuntimeExtension struct
   *  LCEvent runtime extension declaration in Marlin
   *
   *  @see EventExtension
   */
  struct RuntimeExtension : public lcrtrel::LCOwnedExtension<RuntimeExtension, EventExtension> {} ;
}

#endif
