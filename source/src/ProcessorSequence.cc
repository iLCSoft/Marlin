#include "marlin/ProcessorSequence.h"
#include "marlin/Processor.h"
#include "marlin/EventModifier.h"
#include "marlin/Exceptions.h"
#include "marlin/EventExtensions.h"

// -- std headers
#include <algorithm>
#include <iomanip>

namespace marlin {

  bool ProcessorSequence::exists( std::shared_ptr<Processor> processor ) const  {
    return ( nullptr == processor ? false : (_processors.end() != find( processor ) ) ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorSequence::add( std::shared_ptr<Processor> processor ) {
    if( nullptr == processor ) {
      throw Exception( "ProcessorSequence::add: invalid processor ptr" ) ;
    }
    if( exists( processor ) ) {
      throw Exception( "ProcessorSequence::add: processor already in sequence" ) ;
    }
    _processors.push_back( processor ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorSequence::insertAfter( const std::string &afterName, std::shared_ptr<Processor> processor ) {
    if( nullptr == processor ) {
      throw Exception( "ProcessorSequence::insertAfter: invalid processor ptr" ) ;
    }
    if( exists( processor ) ) {
      throw Exception( "ProcessorSequence::insertAfter: processor already in sequence" ) ;
    }
    auto iter = find( afterName ) ;
    if( _processors.end() == iter ) {
      throw Exception( "ProcessorSequence::insertAfter: no processor '" + afterName + "' in sequence" ) ;
    }
    _processors.insert( std::next( iter ), processor ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorSequence::insertBefore( const std::string &afterName, std::shared_ptr<Processor> processor ) {
    if( nullptr == processor ) {
      throw Exception( "ProcessorSequence::insertBefore: invalid processor ptr" ) ;
    }
    if( exists( processor ) ) {
      throw Exception( "ProcessorSequence::insertBefore: processor already in sequence" ) ;
    }
    auto iter = find( afterName ) ;
    if( _processors.end() == iter ) {
      throw Exception( "ProcessorSequence::insertBefore: no processor '" + afterName + "' in sequence" ) ;
    }
    _processors.insert( iter, processor ) ;
  }

  //--------------------------------------------------------------------------

  ProcessorSequence::ConstIterator ProcessorSequence::find( std::shared_ptr<Processor> processor ) const {
    return find( processor->name() ) ;
  }

  //--------------------------------------------------------------------------

  ProcessorSequence::ConstIterator ProcessorSequence::find( const std::string &name ) const {
    return std::find_if( _processors.begin(), _processors.end(), [&name]( std::shared_ptr<Processor> p ) {
      return ( p->name() == name ) ;
    }) ;
  }

  //--------------------------------------------------------------------------

  bool ProcessorSequence::remove( std::shared_ptr<Processor> processor ) {
    auto iter = find( processor ) ;
    if( _processors.end() != iter ) {
      _processors.erase( iter ) ;
      return true ;
    }
    return false ;
  }

  //--------------------------------------------------------------------------

  void ProcessorSequence::processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    for ( auto processor : _processors ) {
      processor->processRunHeader( rhdr.get() ) ;
    }
  }

  //--------------------------------------------------------------------------

  void ProcessorSequence::modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    for ( auto processor : _processors ) {
      auto eventModifier = dynamic_cast<EventModifier*>( processor.get() ) ;
      if ( eventModifier ) {
        eventModifier->modifyRunHeader( rhdr.get() ) ;
      }
    }
  }

  //--------------------------------------------------------------------------

  void ProcessorSequence::processEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    try {
      auto extension = event->runtime().ext<ProcessorConditions>() ;
      for ( auto processor : _processors ) {
        if ( not extension->check( processor->name() ) ) {
          continue ;
        }
        auto startTime = clock() ;
        processor->processEvent( event.get() ) ;
        // if( not _suppressCheck ) {
        processor->check( event.get() ) ;
        // }
        auto endTime = clock() ;
        auto iter = _processorTimes.find( processor->name() ) ;
        iter->second._processingTime += static_cast<double>( endTime - startTime ) ;
        iter->second._eventCounter ++ ;
        // TODO need to deal with that issue ...
        // processor->setFirstEvent( false ) ;
      }
    }
    catch ( SkipEventException& e ) {
      auto iter = _skipEventMap.find( e.what() ) ;
      if ( _skipEventMap.end() == iter ) {
        _skipEventMap.insert( SkippedEventMap::value_type( e.what() , 1 ) ) ;
      }
      else {
        iter->second ++;
      }
    }
  }

  //--------------------------------------------------------------------------

  void ProcessorSequence::modifyEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    auto extension = event->runtime().ext<ProcessorConditions>() ;
    for ( auto processor : _processors ) {
      auto eventModifier = dynamic_cast<EventModifier*>( processor.get() ) ;
      if ( nullptr == eventModifier ) {
        continue ;
      }
      // check runtime condition
      if ( not extension->check( processor->name() ) ) {
        continue ;
      }
      auto startTime = clock() ;
      eventModifier->modifyEvent( event.get() ) ;
      auto endTime = clock() ;
      auto iter = _processorTimes.find( processor->name() ) ;
      iter->second._processingTime += static_cast<double>( endTime - startTime ) ;
    }
  }

  //--------------------------------------------------------------------------

  ProcessorSequence::TimeMetadata ProcessorSequence::generateTimeSummary() const {
    TimeMetadata summary {} ;
    for ( auto t : _processorTimes ) {
      summary._processingTime += t.second._processingTime / double(CLOCKS_PER_SEC) ;
      summary._eventCounter += t.second._eventCounter ;
    }
    return summary ;
  }

} // namespace marlin
