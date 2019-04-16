#include <marlin/EventContext.h>

// -- marlin headers
#include <marlin/Processor.h>

namespace marlin {

  EventContext::Event EventContext::event() const {
    return _event ;
  }

  //--------------------------------------------------------------------------

  EventContext::RandomSeedType EventContext::randomSeed( const Processor *const processor ) const {
    RandomSeedManager::HashFunction hashf ;
    RandomSeedManager::HashResult hashval = hashf( processor ) ;
    auto iter = _randomSeeds->find( hashval ) ;
    if( _randomSeeds->end() == iter ) {
      throw Exception( "EventContext::randomSeed: processor '" +
        processor->name() +
        "' not registered for random seed manager" ) ;
    }
    return iter->second ;
  }

  //--------------------------------------------------------------------------

  void EventContext::setRuntimeCondition( const Processor *const processor, bool value ) {
    _runtimeConditions->setValue( processor->name(), value ) ;
  }

  //--------------------------------------------------------------------------

  void EventContext::setRuntimeCondition( const Processor *const processor, const std::string &name, bool value ) {
    _runtimeConditions->setValue( processor->name() + "." + name, value ) ;
  }

  //--------------------------------------------------------------------------

  bool EventContext::checkRuntimeCondition( const std::string &name ) const {
    return _runtimeConditions->conditionIsTrue( name ) ;
  }

  //--------------------------------------------------------------------------

  std::thread::id EventContext::threadID() const {
    return _threadID ;
  }

  //--------------------------------------------------------------------------

  void EventContext::setThreadID() {
    _threadID = std::this_thread::get_id() ;
  }

  //--------------------------------------------------------------------------

  void EventContext::setEvent( Event evt ) {
    _event = evt ;
  }

  //--------------------------------------------------------------------------

  void EventContext::setRandomSeeds( RandomSeedMap seeds ) {
    _randomSeeds = std::move(seeds) ;
  }

  //--------------------------------------------------------------------------

  void EventContext::setupRuntimeConditions( const std::map<std::string, std::string> &conds ) {
    for( auto cond : conds ) {
      _runtimeConditions->addCondition( cond.first, cond.second ) ;
    }
  }

}
