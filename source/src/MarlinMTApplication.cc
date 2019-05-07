#include <marlin/MarlinMTApplication.h>

// -- marlin headers
#include <marlin/Utils.h>
#include <marlin/DataSourcePlugin.h>
#include <marlin/PluginManager.h>
#include <marlin/concurrency/PEPScheduler.h>

using namespace std::placeholders ;

namespace marlin {

  void MarlinMTApplication::runApplication() {
    try {
      _dataSource->readAll() ;
    }
    catch( StopProcessingException &e ) {
      logger()->log<ERROR>() << std::endl
          << " **********************************************************" << std::endl
          << " *                                                        *" << std::endl
          << " *   Stop of EventProcessing requested by processor :     *" << std::endl
          << " *                  "  << e.what()                           << std::endl
          << " *     will call end() method of all processors !         *" << std::endl
          << " *                                                        *" << std::endl
          << " **********************************************************" << std::endl
          << std::endl ;
    }
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::init() {
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    configureScheduler() ;
    configureDataSource() ;
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::end() {
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    _scheduler->end() ;
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::configureScheduler() {
    _scheduler = std::make_shared<concurrency::PEPScheduler>() ;
    _scheduler->init( this ) ;
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::configureDataSource() {
    // configure the data source
    auto parameters = dataSourceParameters() ;
    auto dstype = parameters->getValue<std::string>( "DataSourceType" ) ;
    auto &pluginMgr = PluginManager::instance() ;
    _dataSource = pluginMgr.create<DataSourcePlugin>( PluginType::DataSource, dstype ) ;
    if( nullptr == _dataSource ) {
      throw Exception( "Data source of type '" + dstype + "' not found in plugins" ) ;
    }
    _dataSource->init( this ) ;
    // setup callbacks
    _dataSource->onEventRead( std::bind( &MarlinMTApplication::onEventRead, this, _1 ) ) ;
    _dataSource->onRunHeaderRead( std::bind( &MarlinMTApplication::onRunHeaderRead, this, _1 ) ) ;
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::onEventRead( std::shared_ptr<EVENT::LCEvent> event ) {
    EventList events ;
    while( _scheduler->freeSlots() == 0 ) {
      _scheduler->popFinishedEvents( events ) ;
      if( not events.empty() ) {
        processFinishedEvents( events ) ;
        events.clear() ;
        break;
      }
      std::this_thread::sleep_for( std::chrono::milliseconds(1) ) ;
    }
    _scheduler->pushEvent( event ) ;
    // check a second time
    _scheduler->popFinishedEvents( events ) ;
    if( not events.empty() ) {
      processFinishedEvents( events ) ;
    }
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::onRunHeaderRead( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    logger()->log<MESSAGE9>() << "New run header no " << rhdr->getRunNumber() << std::endl ;
    _scheduler->processRunHeader( rhdr ) ;
  }

  //--------------------------------------------------------------------------

  void MarlinMTApplication::processFinishedEvents( const EventList &events ) const {
    // simple printout for the time being
    for( auto event : events ) {
      logger()->log<MESSAGE9>()
        << "Run no " << event->getRunNumber()
        << ", event no " << event->getEventNumber()
        << " finished" << std::endl ;
    }
  }

} // namespace marlin
