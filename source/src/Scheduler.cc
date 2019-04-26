#include "marlin/Scheduler.h"
#include "marlin/Processor.h"
#include "marlin/Application.h"
#include "marlin/PluginManager.h"
#include "marlin/EventModifier.h"

// -- std headers
#include <set>
#include <algorithm>
#include <iomanip>

namespace marlin {
  
  Scheduler::Scheduler() {
    _runtimeConditions = std::make_shared<LogicalExpressions>() ;
    _rdmSeedGenerator = std::make_shared<SeedGeneratorType>() ;
    std::stringstream ss ; ss << "Scheduler_" << (void*) this ;
    _logger = Logging::createLogger( ss.str() ) ;
    _logger->setLevel<MESSAGE>() ;
  }
  
  //--------------------------------------------------------------------------
  
  void Scheduler::init( Application *application ) {
    _application = application ;
    std::stringstream ss ; ss << "Scheduler_" << (void*) this ;
    _logger = app().createLogger( ss.str() ) ;
    logger()->log<DEBUG5>() << "Scheduler init ..." << std::endl ;
    // Extract global settings
    auto parameters = app().globalParameters() ;
    _suppressCheck = not (parameters->getStringVal( "SupressCheck" ) != "true" ) ;
    _allowModify = ( parameters->getStringVal( "AllowToModifyEvent" ) == "true" ) ;
    // create list of active processors
    auto activeProcessors = app().activeProcessors() ;
    auto processorConditions = app().processorConditions() ;
    const bool useConditions = ( activeProcessors.size() == processorConditions.size() ) ;
    if ( activeProcessors.empty() ) {
      throw Exception( "Scheduler::init: Active processor list is empty !" ) ;
    }
    auto &mgr = PluginManager::instance() ;
    std::set<std::string> duplicateCheck ( activeProcessors.begin() , activeProcessors.end() ) ;
    if ( duplicateCheck.size() != activeProcessors.size() ) {
      logger()->log<ERROR>() << "Scheduler::init: the following list of active processors are found to be duplicated :" << std::endl ;
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
      logger()->log<DEBUG5>() << "Active processor " << procName << std::endl ;
      auto processorParameters = app().processorParameters( procName ) ;
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
    // Initialize processors
    for ( auto processor : _processors ) {
      logger()->log<DEBUG5>() << "Init processor " << processor->name() << std::endl ;
      processor->setScheduler( this ) ;
      processor->baseInit( application ) ;
      _processorTimes[ processor->name() ] = TimeMetadata() ;
    }
    logger()->log<DEBUG5>() << "Scheduler init ... DONE" << std::endl ;
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
        auto startTime = clock() ;
        processor->processEvent( event ) ;
        if( not _suppressCheck ) {
          processor->check( event ) ;
        }
        auto endTime = clock() ;
        auto iter = _processorTimes.find( processor->name() ) ;
        iter->second._processingTime += static_cast<double>( endTime - startTime ) ;
        iter->second._eventCounter ++ ;
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
        auto startTime = clock() ;
        processor->processEvent( event ) ;
        if( not _suppressCheck ) {
          processor->check( event ) ;
        }
        auto endTime = clock() ;
        auto iter = _processorTimes.find( processor->name() ) ;
        iter->second._processingTime += static_cast<double>( endTime - startTime ) ;
        iter->second._eventCounter ++ ;
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
  
  void Scheduler::modifyRunHeader( EVENT::LCRunHeader *header ) {
    for ( auto processor : _processors ) {
      auto eventModifier = dynamic_cast<EventModifier*>( processor.get() ) ;
      if ( eventModifier ) {
        eventModifier->modifyRunHeader( header ) ;
      }
    }
  }
  
  //--------------------------------------------------------------------------
  
  void Scheduler::end() {
    for ( auto iter = _processors.rbegin(), endIter = _processors.rend() ; endIter != iter ; ++iter ) {
      auto processor = *iter ;
      processor->end() ;
    }
    logger()->log<MESSAGE>() << " --------------------------------------------------------- " << std::endl ;
    logger()->log<MESSAGE>() << "  Events skipped by processors : " << std::endl ;
    unsigned int nSkipped = 0 ;
    for( auto skip : _skipEventMap ) {
      logger()->log<MESSAGE>() << "       " << skip.first << ": \t" << skip.second << std::endl ;
      nSkipped += skip.second ;	
    }
    logger()->log<MESSAGE>() << "  Total: " << nSkipped  << std::endl ;
    logger()->log<MESSAGE>() << " --------------------------------------------------------- "  << std::endl << std::endl ;
    
    logger()->log<MESSAGE>() << " --------------------------------------------------------- " << std::endl
              << "      Time used by processors ( in processEvent() ) :      " << std::endl 
                                                                               << std::endl ;
    std::list<TimeMap::value_type> timeList( _processorTimes.begin() , _processorTimes.end() ) ;
    typedef std::list<TimeMap::value_type>::value_type elt ;
    timeList.sort( [](const elt &lhs, const elt &rhs) {
      return ( lhs.second._processingTime > rhs.second._processingTime ) ;
    }) ;
    double timeTotal = 0.0 ;
    int eventTotal = 0 ;
    for( auto timeInfo : timeList ) {
      std::string procName = timeInfo.first ;
      procName.resize(40, ' ') ;
      double timeProc = timeInfo.second._processingTime  / double(CLOCKS_PER_SEC) ;
      timeTotal += timeProc ;
      int eventProc = timeInfo.second._eventCounter ;
      if( eventProc > eventTotal ){
        eventTotal = eventProc ;
      }
      std::stringstream ss ;
      if ( eventProc > 0 ) {
        ss << timeProc / eventProc ;
      }
      else {
        ss << "NaN" ;
      }
      logger()->log<MESSAGE>() 
        //<< std::left << std::setw(30)
        << procName
        << std::setw(12) << std::scientific  << timeProc  << " s "
        << "in " << std::setw(12) << eventProc 
        << " events  ==> " 
        << std::setw(12) << std::scientific << ss.str() << " [ s/evt.] "
        << std::endl << std::endl ;
    }
    std::stringstream ss ;
    if ( eventTotal > 0 ) {
      ss << timeTotal / eventTotal ;
    }
    else {
      ss << "NaN" ;
    }
    logger()->log<MESSAGE>() 
      << "Total:                                  "
      <<  std::setw(12) << std::scientific  << timeTotal << " s in "
      <<  std::setw(12) << eventTotal << " events  ==> " 
      <<  std::setw(12) << std::scientific << ss.str() << " [ s/evt.] "
      << std::endl << std::endl ;
    logger()->log<MESSAGE>()  << " --------------------------------------------------------- "  << std::endl ;
  }
  
  //--------------------------------------------------------------------------
  
  std::shared_ptr<LogicalExpressions> Scheduler::runtimeConditions() const { 
    return _runtimeConditions ; 
  }
  
  //--------------------------------------------------------------------------
  
  std::shared_ptr<Scheduler::SeedGeneratorType> Scheduler::randomSeedGenerator() const { 
    return _rdmSeedGenerator ; 
  }
  
  //--------------------------------------------------------------------------
  
  Scheduler::Logger Scheduler::logger() const {
    return _logger ;
  }
  
  //--------------------------------------------------------------------------
  
  const Application &Scheduler::app() const {
    if ( nullptr == _application ) {
      throw Exception( "Scheduler::app: not initialized !" ) ;
    }
    return *_application ;
  }
  
  //--------------------------------------------------------------------------
  
  Scheduler::TimeMetadata Scheduler::generateTimeSummary() const {
    TimeMetadata summary {} ;
    for ( auto t : _processorTimes ) {
      summary._processingTime += t.second._processingTime / double(CLOCKS_PER_SEC) ;
      summary._eventCounter += t.second._eventCounter ;
    }
    return summary ;
  }

} // namespace marlin

