#include <marlin/EventExtensions.h>

// -- marlin headers
#include <marlin/Processor.h>

namespace marlin {

  EventExtension::RandomSeedType EventExtension::randomSeed( const Processor *const processor ) const {
    RandomSeedManager::HashFunction hashf ;
    RandomSeedManager::HashResult hashval = hashf( processor ) ;
    auto iter = _randomSeeds->find( hashval ) ;
    if( _randomSeeds->end() == iter ) {
      throw Exception( "EventExtension::randomSeed: processor '" +
        processor->name() +
        "' not registered for random seed manager" ) ;
    }
    return iter->second ;
  }

  //--------------------------------------------------------------------------

  void EventExtension::setRuntimeCondition( const Processor *const processor, bool value ) {
    _runtimeConditions->setValue( processor->name(), value ) ;
  }

  //--------------------------------------------------------------------------

  void EventExtension::setRuntimeCondition( const Processor *const processor, const std::string &name, bool value ) {
    _runtimeConditions->setValue( processor->name() + "." + name, value ) ;
  }

  //--------------------------------------------------------------------------

  bool EventExtension::checkRuntimeCondition( const std::string &name ) const {
    return _runtimeConditions->conditionIsTrue( name ) ;
  }

  //--------------------------------------------------------------------------

  void EventExtension::setRandomSeeds( RandomSeedMap seeds ) {
    _randomSeeds = std::move(seeds) ;
  }

  //--------------------------------------------------------------------------

  void EventExtension::setupRuntimeConditions( const std::map<std::string, std::string> &conds ) {
    for( auto cond : conds ) {
      _runtimeConditions->addCondition( cond.first, cond.second ) ;
    }
  }

}
