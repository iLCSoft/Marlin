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
   *  @brief  ProcessorConditionsExtension class
   *  Event extension providing access to processor runtime conditions (LogicalExpressions)
   */
  class ProcessorConditionsExtension {
  public:
    using Conditions = std::shared_ptr<LogicalExpressions> ;
    using ConditionsMap = std::map<std::string, std::string> ;

  public:
    ~ProcessorConditionsExtension() = default ;
    ProcessorConditionsExtension() = delete ;
    ProcessorConditionsExtension(const ProcessorConditionsExtension&) = delete ;
    ProcessorConditionsExtension& operator=(const ProcessorConditionsExtension&) = delete ;

  public:
    /**
     *  @brief  Constructor
     *
     *  @param  conds the initial runtime condition of the event (from steering file)
     */
    ProcessorConditionsExtension( const ConditionsMap &conds ) ;

    /**
     *  @brief  Set the runtime condition of the processor
     *
     *  @param  processor the processor pointer
     *  @param  value the runtime condition to set
     */
    void set( const Processor *const processor, bool value ) ;

    /**
     *  @brief  Set the named runtime condition of the processor
     *
     *  @param  processor the processor pointer
     *  @param  name the condition name
     *  @param  value the runtime condition to set
     */
    void set( const Processor *const processor, const std::string &name, bool value ) ;

    /**
     *  @brief  Check whether the runtime condition is true
     *
     *  @param  name the condition name
     */
    bool check( const std::string &name ) const ;

  private:
    /// The runtime conditions
    Conditions     _runtimeConditions {std::make_shared<LogicalExpressions>()} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  /**
   *  @brief  RandomSeedExtension class
   *  Event extension providing access to random seeds
   */
  class RandomSeedExtension {
  public:
    using RandomSeedMap = std::unique_ptr<RandomSeedManager::RandomSeedMap> ;
    using RandomSeedType = RandomSeedManager::SeedType ;

  public:
    ~RandomSeedExtension() = default ;
    RandomSeedExtension() = delete ;
    RandomSeedExtension(const RandomSeedExtension&) = delete ;
    RandomSeedExtension& operator=(const RandomSeedExtension&) = delete ;

  public:
    /**
     *  @brief  Constructor
     *
     *  @param  seeds the random seed set for the current event
     */
    RandomSeedExtension( RandomSeedMap seeds ) ;

    /**
     *  @brief  Get the random seed for a given processor
     *
     *  @param  processor the processor pointer
     */
    RandomSeedType randomSeed( const Processor *const processor ) const ;

  private:
    /// The random seed map for the current event
    RandomSeedMap         _randomSeeds {nullptr} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  // LCEvent runtime extension declaration in Marlin
  struct ProcessorConditions : public lcrtrel::LCOwnedExtension<ProcessorConditions, ProcessorConditionsExtension> {} ;
  struct RandomSeed : public lcrtrel::LCOwnedExtension<RandomSeed, RandomSeedExtension> {} ;
}

#endif
