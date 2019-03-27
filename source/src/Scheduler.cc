#include "marlin/Scheduler.h"
#include "marlin/Processor.h"
#include "marlin/Application.h"
#include "marlin/PluginManager.h"
#include "marlin/EventModifier.h"

// -- std headers
#include <set>
#include <algorithm>

namespace marlin {
  
  Scheduler::Scheduler() {
    _runtimeConditions = std::make_shared<LogicalExpressions>() ;
    _rdmSeedGenerator = std::make_shared<SeedGeneratorType>() ;
  }
  
  //--------------------------------------------------------------------------
  
  void Scheduler::init( Application *app ) {
    // Extract global settings
    auto parameters = app->globalParameters() ;
    _suppressCheck = not (parameters->getStringVal( "SupressCheck" ) != "true" ) ;
    _allowModify = ( parameters->getStringVal( "AllowToModifyEvent" ) == "true" ) ;
    // create list of active processors
    auto activeProcessors = app->activeProcessors() ;
    auto processorConditions = app->processorConditions() ;
    const bool useConditions = ( activeProcessors.size() == processorConditions.size() ) ;
    if ( activeProcessors.empty() ) {
      throw Exception( "Scheduler::init: Active processor list is empty !" ) ;
    }
    auto &mgr = PluginManager::instance() ;
    std::set<std::string> duplicateCheck ( activeProcessors.begin() , activeProcessors.end() ) ;
    if ( duplicateCheck.size() != activeProcessors.size() ) {
      std::cout << "Scheduler::init: the following list of active processors are found to be duplicated :" << std::endl ;
      for ( auto procName : activeProcessors ) {
        auto c = std::count( activeProcessors.begin() , activeProcessors.end() , procName ) ;
        if( c > 1 ) {
          std::cout << "   * " << procName << " (" << c << " instances)" << std::endl ;
        }
      }
      throw Exception( "Scheduler::init: duplicated active processors. Check your steering file !" ) ;
    }
    for ( size_t i=0 ; i<activeProcessors.size() ; ++i ) {
      auto procName = activeProcessors[ i ] ;
      auto processorParameters = app->processorParameters( procName ) ;
      if ( nullptr == processorParameters ) {
        throw Exception( "Scheduler::init: undefined processor '" + procName + "'" ) ;
      }
      auto procType = processorParameters->getStringVal("ProcessorType") ;
      auto processor = mgr.create<Processor>( PluginType::Processor, procType ) ;
      processor->setName( procName ) ;
      if ( nullptr == processor ) {
        throw Exception( "Scheduler::init: processor '" + procType + "' not registered !" ) ; 
      }
      _processors.push_back( processor ) ;
      if ( useConditions ) {
        _runtimeConditions->addCondition( procName, processorConditions[ i ] ) ;
      }
      processor->setParameters( processorParameters ) ;
    }
    // TODO configure logging for this processor chain here
    // Initialize processors
    for ( auto processor : _processors ) {
      processor->setScheduler( this ) ;
      processor->baseInit() ;
      _processorTimes[ processor->name() ] = TimeMetadata() ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void Scheduler::processEvent( EVENT::LCEvent *event ) {
    _runtimeConditions->clear() ;
    // already called from modifyEvent, skip it !
    if ( _allowModify ) {
      return ;
    }
    _rdmSeedGenerator->refreshSeeds( event ) ;
    try {
      // Process events now
      for ( auto processor : _processors ) {
        if ( not _runtimeConditions->conditionIsTrue( processor->name() ) ) {
          continue ;
        }
        // TODO setup logger here
        auto startTime = clock() ;
        processor->processEvent( event ) ;
        if( not _suppressCheck ) {
          processor->check( event ) ;
        }
        auto endTime = clock() ;
        auto iter = _processorTimes.find( processor->name() ) ;
        iter->second._processingTime += static_cast<double>( endTime - startTime ) ;
        iter->second._eventCounter ++ ;
        processor->setFirstEvent( false ) ;
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
  
  void Scheduler::processRunHeader( EVENT::LCRunHeader *header ) {
    for ( auto processor : _processors ) {
      processor->processRunHeader( header ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void Scheduler::modifyEvent( EVENT::LCEvent *event ) {
    _runtimeConditions->clear() ;
    _rdmSeedGenerator->refreshSeeds( event ) ;
    for ( auto processor : _processors ) {
      auto eventModifier = dynamic_cast<EventModifier*>( processor.get() ) ;
      if ( nullptr == eventModifier ) {
        continue ;
      }
      // check runtime condition
      if ( not _runtimeConditions->conditionIsTrue( processor->name() ) ) {
        continue ;
      }
      // TODO setup logger correctly here
      // logger->setLogLevel() ;
      auto startTime = clock() ;
      eventModifier->modifyEvent( event ) ;
      auto endTime = clock() ;
      auto iter = _processorTimes.find( processor->name() ) ;
      iter->second._processingTime += static_cast<double>( endTime - startTime ) ;
    }
    // Call processEvent here only if settings says that users are 
    // allowed to modify events in processEvent calls 
    if( not _allowModify ) {
      return ;
    }
    // refresh seeds again
    _rdmSeedGenerator->refreshSeeds( event ) ;
    try {
      // Process events now
      for ( auto processor : _processors ) {
        if ( not _runtimeConditions->conditionIsTrue( processor->name() ) ) {
          continue ;
        }
        // TODO setup logger here
        auto startTime = clock() ;
        processor->processEvent( event ) ;
        if( not _suppressCheck ) {
          processor->check( event ) ;
        }
        auto endTime = clock() ;
        auto iter = _processorTimes.find( processor->name() ) ;
        iter->second._processingTime += static_cast<double>( endTime - startTime ) ;
        iter->second._eventCounter ++ ;
        processor->setFirstEvent( false ) ;
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
  
  void Scheduler::modifyRunHeader( EVENT::LCRunHeader *header ) {
    for ( auto processor : _processors ) {
      // TODO handle logging here
      auto eventModifier = dynamic_cast<EventModifier*>( processor.get() ) ;
      if ( eventModifier ) {
        eventModifier->modifyRunHeader( header ) ;
      }
    }
  }
  
  //--------------------------------------------------------------------------
  
  void Scheduler::end() {
    for ( auto iter = _processors.rbegin(), endIter = _processors.rend() ; endIter != iter ; ++iter ) {
      // TODO setup logger here
      auto processor = *iter ;
      processor->end() ;
    }
    std::cout << " --------------------------------------------------------- " << std::endl ;
    std::cout << "  Events skipped by processors : " << std::endl ;
    unsigned int nSkipped = 0 ;
    for( auto skip : _skipEventMap ) {
      std::cout << "       " << skip.first << ": \t" << skip.second << std::endl ;
      nSkipped += skip.second ;	
    }
    std::cout << "  Total: " << nSkipped  << std::endl ;
    std::cout << " --------------------------------------------------------- "  << std::endl << std::endl ;
    
    std::cout << " --------------------------------------------------------- " << std::endl
              << "      Time used by processors ( in processEvent() ) :      " << std::endl 
                                                                               << std::endl ;
    std::list<TimeMap::value_type> timeList( _processorTimes.begin() , _processorTimes.end() ) ;
    typedef std::list<TimeMap::value_type>::value_type elt ;
    timeList.sort( [](const elt &lhs, const elt &rhs) {
      return ( lhs.second._processingTime > rhs.second._processingTime ) ;
    }) ;
  }
  
  //--------------------------------------------------------------------------
  
  std::shared_ptr<LogicalExpressions> Scheduler::runtimeConditions() const { 
    return _runtimeConditions ; 
  }
  
  //--------------------------------------------------------------------------
  
  std::shared_ptr<Scheduler::SeedGeneratorType> Scheduler::randomSeedGenerator() const { 
    return _rdmSeedGenerator ; 
  }

} // namespace marlin

