#include <marlin/SimpleScheduler.h>

// -- marlin headers
#include <marlin/Application.h>
#include <marlin/Utils.h>
#include <marlin/Sequence.h>
#include <marlin/Processor.h>

// -- std headers
#include <algorithm>
#include <set>

namespace marlin {

  void SimpleScheduler::init( const Application *app ) {
    _logger = app->createLogger( "SimpleScheduler" ) ;
    auto activeProcessors = app->activeProcessors() ;
    // create super sequence with only only sequence and fill it
    _superSequence = std::make_shared<SuperSequence>(1) ;
    _logger->log<DEBUG5>() << "Creating processors ..." << std::endl ;
    if ( activeProcessors.empty() ) {
      throw Exception( "SimpleScheduler::init: Active processor list is empty !" ) ;
    }
    // check for duplicates first
    std::set<std::string> duplicateCheck ( activeProcessors.begin() , activeProcessors.end() ) ;
    if ( duplicateCheck.size() != activeProcessors.size() ) {
      _logger->log<ERROR>() << "SimpleScheduler::init: the following list of active processors are found to be duplicated :" << std::endl ;
      for ( auto procName : activeProcessors ) {
        auto c = std::count( activeProcessors.begin() , activeProcessors.end() , procName ) ;
        if( c > 1 ) {
          _logger->log<ERROR>() << "   * " << procName << " (" << c << " instances)" << std::endl ;
        }
      }
      throw Exception( "SimpleScheduler::init: duplicated active processors. Check your steering file !" ) ;
    }
    // populate processor sequences
    for ( size_t i=0 ; i<activeProcessors.size() ; ++i ) {
      auto procName = activeProcessors[ i ] ;
      _logger->log<DEBUG5>() << "Active processor " << procName << std::endl ;
      auto processorParameters = app->processorParameters( procName ) ;
      if ( nullptr == processorParameters ) {
        throw Exception( "SimpleScheduler::init: undefined processor '" + procName + "'" ) ;
      }
      _superSequence->addProcessor( processorParameters ) ;
    }
    _logger->log<DEBUG5>() << "Creating processors ... OK" << std::endl ;
  }
  
  //--------------------------------------------------------------------------
  
  void SimpleScheduler::end() {
    _logger->log<MESSAGE>() << "Terminating application" << std::endl ;
    _superSequence->end() ;
  }
  
  //--------------------------------------------------------------------------
  
  void SimpleScheduler::processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    _superSequence->modifyRunHeader( rhdr ) ;
    _superSequence->processRunHeader( rhdr ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void SimpleScheduler::pushEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    _currentEvent = event ;
    auto sequence = _superSequence->sequence(0) ;
    sequence->modifyEvent( _currentEvent ) ;
    sequence->processEvent( _currentEvent ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void SimpleScheduler::popFinishedEvents( std::vector<std::shared_ptr<EVENT::LCEvent>> &events ) {
    if( nullptr != _currentEvent ) {
      events.push_back( _currentEvent ) ;
      _currentEvent = nullptr ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  std::size_t SimpleScheduler::freeSlots() const {
    return ( _currentEvent != nullptr ) ? 0 : 1 ;
  }

}
