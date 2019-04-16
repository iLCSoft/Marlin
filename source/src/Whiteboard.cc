#include <marlin/Whiteboard.h>

// -- marlin headers
#include <marlin/EventContext.h>
#include <marlin/Exceptions.h>
#include <marlin/Application.h>

namespace marlin {

  void Whiteboard::init( const Application &app ) {
    // get the random seed from steering file if any
    if( app.globalParameters()->isParameterSet( "RandomSeed" ) ) {
      RandomSeedManager::SeedType randomSeed = std::abs( app.globalParameters()->getIntVal( "RandomSeed" ) ) ;
      _rndSeedMgr = std::make_shared<RandomSeedManager>( randomSeed ) ;
    }
    else {
      _rndSeedMgr = std::make_shared<RandomSeedManager>() ;
    }
    // get processor conditions from steering file
    auto processorConditions = app.processorConditions() ;
    if( not processorConditions.empty() ) {
      auto activeProcessors = app.activeProcessors() ;
      for( unsigned int i=0 ; i<activeProcessors.size() ; i++ ) {
        _initialConditions[ activeProcessors.at(i) ] = processorConditions.at(i) ;
      }
    }
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<EventContext> Whiteboard::createEventContext( std::shared_ptr<EVENT::LCEvent> event ) {
    auto eventContext = std::make_shared<EventContext>() ;
    eventContext->setEvent( event ) ;
    eventContext->setRandomSeeds( std::move( _rndSeedMgr->generateRandomSeeds( event.get() ) ) ) ;
    eventContext->setupRuntimeConditions( _initialConditions ) ;
    {
      std::lock_guard<std::mutex> lock( _mutex ) ;
      auto iter = _eventContexts.find( event ) ;
      if( iter != _eventContexts.end() ) {
        throw Exception( "Whiteboard::createEventContext: event context already created" ) ;
      }
      _eventContexts[ event ] = eventContext ;
    }
    return eventContext ;
  }

  //--------------------------------------------------------------------------

  void Whiteboard::removeEventContext( std::shared_ptr<EventContext> ctx ) {
    std::lock_guard<std::mutex> lock( _mutex ) ;
    auto iter = _eventContexts.find( ctx->event() ) ;
    if( iter == _eventContexts.end() ) {
      throw Exception( "Whiteboard::removeEventContext: event context not in whiteboard!" ) ;
    }
    _eventContexts.erase( iter ) ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<EventContext> Whiteboard::retrieveEventContext( std::shared_ptr<EVENT::LCEvent> event ) const {
    std::lock_guard<std::mutex> lock( _mutex ) ;
    auto iter = _eventContexts.find( event ) ;
    return ( _eventContexts.end() == iter ? nullptr : iter->second ) ;
  }

}
