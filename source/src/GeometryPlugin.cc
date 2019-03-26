#include "marlin/GeometryPlugin.h"
#include "marlin/VerbosityLevels.h"

namespace marlin {

  GeometryPlugin::GeometryPlugin( const std::string &gtype ) :
    _type(gtype) {
    // create a standalone logger
    _logger = logstream::createLogger( "Geometry" ) ;  
    // verbosity level for this plugin
    registerOptionalParameter( "Verbosity", 
      "the geometry plugin verbosity level",
      _verbosity,
      "MESSAGE") ;
  }
  
  //--------------------------------------------------------------------------

  void GeometryPlugin::print() const {
    auto typeidx = typeIndex() ;
    _logger.log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    _logger.log<MESSAGE>() << "-- Geometry plugin: " << type() << std::endl ;
    _logger.log<MESSAGE>() << "-- Description: " << description() << std::endl ;
    _logger.log<MESSAGE>() << "-- Handle at: " << handle() << std::endl ;
    _logger.log<DEBUG5>()  << "-- Type index: " << std::endl ;
    _logger.log<DEBUG5>()  << "---- name: " << typeidx.name() << std::endl ;
    _logger.log<DEBUG5>()  << "---- hash: " << typeidx.hash_code() << std::endl ;
    if ( _logger.wouldWrite<DEBUG5>() ) {
      _logger.log<DEBUG5>() << "-- Geometry dump:" << std::endl ;
      dumpGeometry() ;
    }
    _logger.log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }
  
  //--------------------------------------------------------------------------
  
  void GeometryPlugin::init( const Application *application ) {
    _application = application ;
    _logger->setLevel( _verbosity ) ;
    // TODO get the application logger sink and configure 
    // the plugin logger with it
    // _logger = app()->logger() ;
    loadGeometry() ;
    print() ;
  }
  
  //--------------------------------------------------------------------------
  
  const Application &GeometryPlugin::app() const {
    if ( nullptr == _application ) {
      throw Exception( "GeometryPlugin: application is null !" ) ;
    }
    return *_application ;
  }

} // namespace marlin
