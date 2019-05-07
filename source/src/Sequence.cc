#include <marlin/Sequence.h>

// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/EventExtensions.h>
#include <marlin/EventModifier.h>
#include <marlin/StringParameters.h>
#include <marlin/PluginManager.h>

// -- std headers
#include <algorithm>

namespace marlin {

  SequenceItem::SequenceItem( std::shared_ptr<Processor> proc, bool lock ) :
    _processor(proc),
    _mutex(lock ? std::make_shared<std::mutex>() : nullptr) {
    if( nullptr == _processor ) {
      throw Exception( "SequenceItem: got a nullptr for processor" ) ;
    }
  }

  //--------------------------------------------------------------------------

  void SequenceItem::processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    if( nullptr != _mutex ) {
      std::lock_guard<std::mutex> lock( *_mutex ) ;
      _processor->processRunHeader( rhdr.get() ) ;
    }
    else {
      _processor->processRunHeader( rhdr.get() ) ;
    }
  }

  //--------------------------------------------------------------------------

  void SequenceItem::modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    auto modifier = dynamic_cast<EventModifier*>( _processor.get() ) ;
    if( nullptr == modifier ) {
      return ;
    }
    if( nullptr != _mutex ) {
      std::lock_guard<std::mutex> lock( *_mutex ) ;
      modifier->modifyRunHeader( rhdr.get() ) ;
    }
    else {
      modifier->modifyRunHeader( rhdr.get() ) ;
    }
  }

  //--------------------------------------------------------------------------

  SequenceItem::ClockPair SequenceItem::processEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    if( nullptr != _mutex ) {
      clock_t start = clock() ;
      std::lock_guard<std::mutex> lock( *_mutex ) ;
      clock_t start2 = clock() ;
      _processor->processEvent( event.get() ) ;
      _processor->check( event.get() ) ;
      clock_t end = clock() ;
      return ClockPair(end-start, end-start2) ;
    }
    else {
      clock_t start = clock() ;
      _processor->processEvent( event.get() ) ;
      _processor->check( event.get() ) ;
      clock_t end = clock() ;
      return ClockPair(end-start, end-start) ;
    }
  }

  //--------------------------------------------------------------------------

  SequenceItem::ClockPair SequenceItem::modifyEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    auto modifier = dynamic_cast<EventModifier*>( _processor.get() ) ;
    if( nullptr == modifier ) {
      return ClockPair(0, 0) ;
    }
    if( nullptr != _mutex ) {
      clock_t start = clock() ;
      std::lock_guard<std::mutex> lock( *_mutex ) ;
      clock_t start2 = clock() ;
      modifier->modifyEvent( event.get() ) ;
      clock_t end = clock() ;
      return ClockPair(end-start, end-start2) ;
    }
    else {
      clock_t start = clock() ;
      modifier->modifyEvent( event.get() ) ;
      clock_t end = clock() ;
      return ClockPair(end-start, end-start) ;
    }
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<Processor> SequenceItem::processor() const {
    return _processor ;
  }

  //--------------------------------------------------------------------------

  const std::string &SequenceItem::name() const {
    return _processor->name() ;
  }

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  std::shared_ptr<SequenceItem> Sequence::createItem( std::shared_ptr<Processor> processor, bool lock ) const {
    return std::make_shared<SequenceItem>( processor, lock ) ;
  }

  //--------------------------------------------------------------------------

  void Sequence::addItem( std::shared_ptr<SequenceItem> item ) {
    auto iter = std::find_if(_items.begin(), _items.end(), [&](std::shared_ptr<SequenceItem> i){
      return (i->name() == item->name()) ;
    });
    if( _items.end() != iter ) {
      throw Exception( "Sequence::addItem: processor '" + item->name() + "' already in sequence" ) ;
    }
    _items.push_back( item ) ;
    _clockMeasures[item->name()] = ClockMeasure() ;
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<SequenceItem> Sequence::at( Index index ) const {
    return _items.at( index ) ;
  }

  //--------------------------------------------------------------------------

  Sequence::SizeType Sequence::size() const {
    return _items.size() ;
  }

  //--------------------------------------------------------------------------

  void Sequence::processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    for ( auto item : _items ) {
      item->processRunHeader( rhdr ) ;
    }
  }

  //--------------------------------------------------------------------------

  void Sequence::modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    for ( auto item : _items ) {
      item->modifyRunHeader( rhdr ) ;
    }
  }

  //--------------------------------------------------------------------------

  void Sequence::processEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    try {
      auto extension = event->runtime().ext<ProcessorConditions>() ;
      for ( auto item : _items ) {
        if ( not extension->check( item->name() ) ) {
          continue ;
        }
        auto clockMeas = item->processEvent( event ) ;
        auto iter = _clockMeasures.find( item->name() ) ;
        iter->second._appClock += clockMeas.first / static_cast<double>( CLOCKS_PER_SEC ) ;
        iter->second._procClock += clockMeas.second / static_cast<double>( CLOCKS_PER_SEC ) ;
        iter->second._counter ++ ;
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

  void Sequence::modifyEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    auto extension = event->runtime().ext<ProcessorConditions>() ;
    for ( auto item : _items ) {
      // check runtime condition
      if ( not extension->check( item->name() ) ) {
        continue ;
      }
      auto clockMeas = item->modifyEvent( event ) ;
      auto iter = _clockMeasures.find( item->name() ) ;
      iter->second._appClock += clockMeas.first / static_cast<double>( CLOCKS_PER_SEC ) ;
      iter->second._procClock += clockMeas.second / static_cast<double>( CLOCKS_PER_SEC ) ;
    }
  }

  //--------------------------------------------------------------------------

  ClockMeasure Sequence::clockMeasureSummary() const {
    ClockMeasure summary {} ;
    for ( auto t : _clockMeasures ) {
      summary._appClock += t.second._appClock ;
      summary._procClock += t.second._procClock ;
      summary._counter += t.second._counter ;
    }
    return summary ;
  }

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  SuperSequence::SuperSequence( std::size_t nseqs ) {
    if( 0 == nseqs ) {
      throw Exception( "SuperSequence: number of sequences must be > 0" ) ;
    }
    _sequences.resize(nseqs) ;
    for( std::size_t i=0 ; i<nseqs ; ++i ) {
      _sequences.at(i) = std::make_shared<Sequence>() ;
    }
  }

  //--------------------------------------------------------------------------

  void SuperSequence::init( Application *app ) {
    for( auto item : _uniqueItems ) {
      item->processor()->baseInit( app ) ;
    }
  }

  //--------------------------------------------------------------------------

  std::shared_ptr<Sequence> SuperSequence::sequence( Index index ) const {
    return _sequences.at( index ) ;
  }

  //--------------------------------------------------------------------------

  void SuperSequence::addProcessor( std::shared_ptr<StringParameters> parameters ) {
    const bool cloneSet = parameters->isParameterSet( "ProcessorClone" ) ;
    const bool criticalSet = parameters->isParameterSet( "ProcessorCritical" ) ;
    bool clone = parameters->getValue<bool>( "ProcessorClone", true ) ;
    bool critical = parameters->getValue<bool>( "ProcessorCritical", false ) ;
    auto type = parameters->getValue<std::string>( "ProcessorType" ) ;
    auto &pluginMgr = PluginManager::instance() ;
    auto processor = pluginMgr.create<Processor>( PluginType::Processor, type ) ;
    if( nullptr == processor ) {
      throw Exception( "Processor of type '" + type + "' doesn't exists !" ) ;
    }
    auto cloneOpt = processor->getForcedRuntimeOption( Processor::RuntimeOption::Clone ) ;
    auto criticalOpt = processor->getForcedRuntimeOption( Processor::RuntimeOption::Critical ) ;
    if( cloneOpt.first ) {
      if( cloneSet and (cloneOpt.second != clone) ) {
        throw Exception( "Processor '" +
        type +
        "' clone option forced to " +
        (cloneOpt.second ? "true" : "false") +
        "!") ;
      }
      clone = cloneOpt.second ;
    }
    if( criticalOpt.first ) {
      if( criticalSet and (criticalOpt.second != critical) ) {
        throw Exception( "Processor '" +
        type +
        "' critical option forced to " +
        (criticalOpt.second ? "true" : "false") +
        "!") ;
      }
      critical = criticalOpt.second ;
    }
    processor->setParameters( parameters ) ;
    if( clone ) {
      // add the first but then create new processor instances and add them
      auto item = _sequences.at(0)->createItem( processor, critical ) ;
      _sequences.at(0)->addItem( item ) ;
      _uniqueItems.insert( item ) ;
      for( SizeType i=1 ; i<size() ; ++i ) {
        processor = pluginMgr.create<Processor>( PluginType::Processor, type ) ;
        processor->setParameters( parameters ) ;
        item = _sequences.at(i)->createItem( processor, critical ) ;
        _sequences.at(i)->addItem( item ) ;
        _uniqueItems.insert( item ) ;
      }
    }
    else {
      // add the first and re-use the same item
      auto item = _sequences.at(0)->createItem( processor, critical ) ;
      _sequences.at(0)->addItem( item ) ;
      _uniqueItems.insert( item ) ;
      for( SizeType i=1 ; i<size() ; ++i ) {
        _sequences.at(i)->addItem( item ) ;
      }
    }
  }

  //--------------------------------------------------------------------------

  SuperSequence::SizeType SuperSequence::size() const {
    return _sequences.size() ;
  }

  //--------------------------------------------------------------------------

  void SuperSequence::processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    for( auto item : _uniqueItems ) {
      item->processRunHeader( rhdr ) ;
    }
  }

  //--------------------------------------------------------------------------

  void SuperSequence::modifyRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    for( auto item : _uniqueItems ) {
      item->modifyRunHeader( rhdr ) ;
    }
  }

  //--------------------------------------------------------------------------

  void SuperSequence::end() {
    for( auto item : _uniqueItems ) {
      item->processor()->end() ;
    }
  }

}
