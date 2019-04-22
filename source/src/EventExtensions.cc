#include <marlin/EventExtensions.h>

// -- marlin headers
#include <marlin/Processor.h>

namespace marlin {

  RandomSeedExtension::RandomSeedExtension( RandomSeedMap seeds ) :
    _randomSeeds(std::move(seeds)) {
    /* nop */
  }

  //--------------------------------------------------------------------------

  RandomSeedExtension::RandomSeedType RandomSeedExtension::randomSeed( const Processor *const processor ) const {
    RandomSeedManager::HashFunction hashf ;
    RandomSeedManager::HashResult hashval = hashf( processor ) ;
    auto iter = _randomSeeds->find( hashval ) ;
    if( _randomSeeds->end() == iter ) {
      throw Exception( "RandomSeedExtension::randomSeed: processor '" +
        processor->name() +
        "' not registered in random seed manager" ) ;
    }
    return iter->second ;
  }

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  ProcessorConditionsExtension::ProcessorConditionsExtension( const ConditionsMap &conds ) {
    for( auto cond : conds ) {
      _runtimeConditions->addCondition( cond.first, cond.second ) ;
    }
  }

  //--------------------------------------------------------------------------

  void ProcessorConditionsExtension::set( const Processor *const processor, bool value ) {
    _runtimeConditions->setValue( processor->name(), value ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorConditionsExtension::set( const Processor *const processor, const std::string &name, bool value ) {
    _runtimeConditions->setValue( processor->name() + "." + name, value ) ;
  }

  //--------------------------------------------------------------------------

  bool ProcessorConditionsExtension::check( const std::string &name ) const {
    return _runtimeConditions->conditionIsTrue( name ) ;
  }

}
