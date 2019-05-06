#include "marlin/MarlinApplication.h"

// -- marlin headers
#include <marlin/Utils.h>
#include <marlin/DataSourcePlugin.h>
#include <marlin/PluginManager.h>
#include <marlin/SimpleScheduler.h>

using namespace std::placeholders ;

namespace marlin {

  void MarlinApplication::runApplication() {
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

  void MarlinApplication::init() {
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    configureScheduler() ;
    configureDataSource() ;
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }
  
  //--------------------------------------------------------------------------

  void MarlinApplication::end() {
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    _scheduler->end() ;
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void MarlinApplication::printUsage() const {

  }
  
  //--------------------------------------------------------------------------

  void MarlinApplication::configureScheduler() {
    _scheduler = std::make_shared<SimpleScheduler>() ;
    _scheduler->init( this ) ;
  }
  
  //--------------------------------------------------------------------------

  void MarlinApplication::configureDataSource() {
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
    _dataSource->onEventRead( std::bind( &IScheduler::pushEvent, _scheduler.get(), _1 ) ) ;
    _dataSource->onRunHeaderRead( std::bind( &IScheduler::processRunHeader, _scheduler.get(), _1 ) ) ;
  }


} // namespace marlin
