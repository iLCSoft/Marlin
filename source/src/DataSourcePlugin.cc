#include <marlin/DataSourcePlugin.h>

// -- marlin headers
#include <marlin/Application.h>

namespace marlin {

  DataSourcePlugin::DataSourcePlugin( const std::string &dstype ) :
    _type(dstype) {
    _logger = Logging::createLogger( "Data source '" + _type + "'" ) ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::init( const Application *app ) {
    _logger = app->createLogger( "Data source '" + _type + "'" ) ;
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    logger()->log<MESSAGE>() << "Data source" << std::endl ;
    logger()->log<MESSAGE>() << "  - type: " << type() << std::endl ;
    logger()->log<MESSAGE>() << "  - description: " << description() << std::endl ;
    init() ;
    logger()->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }

  //--------------------------------------------------------------------------

  const std::string &DataSourcePlugin::type() const {
    return _type ;
  }

  //--------------------------------------------------------------------------

  const std::string &DataSourcePlugin::description() const {
    return _description ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::readAll() {
    while( readOne() ) ;
  }

  //--------------------------------------------------------------------------

  DataSourcePlugin::Logger DataSourcePlugin::logger() const {
    return _logger ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::onEventRead( EventFunction func ) {
    _onEventRead = func ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::onRunHeaderRead( RunHeaderFunction func ) {
    _onRunHeaderRead = func ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    if( nullptr == _onRunHeaderRead ) {
      throw Exception( "DataSourcePlugin::processRunHeader: no callback function available" ) ;
    }
    _onRunHeaderRead( rhdr ) ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::processEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    if( nullptr == _onEventRead ) {
      throw Exception( "DataSourcePlugin::processEvent: no callback function available" ) ;
    }
    _onEventRead( event ) ;
  }

}
