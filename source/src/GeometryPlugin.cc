#include "marlin/GeometryPlugin.h"

// -- marlin headers
#include "marlin/VerbosityLevels.h"
#include "marlin/PluginManager.h"

namespace marlin {

  GeometryPlugin::GeometryPlugin( const std::string &gtype ) :
    _type(gtype) {
    // create a standalone logger
    _logger = Logging::createLogger( "Geometry" ) ;
    // verbosity level for this plugin
    registerOptionalParameter( "Verbosity",
      "the geometry plugin verbosity level",
      _verbosity,
      std::string("MESSAGE")) ;
  }

  //--------------------------------------------------------------------------

  void GeometryPlugin::print() const {
    auto typeidx = typeIndex() ;
    _logger->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    _logger->log<MESSAGE>() << "-- Geometry plugin: " << type() << std::endl ;
    _logger->log<MESSAGE>() << "-- Description: " << description() << std::endl ;
    _logger->log<MESSAGE>() << "-- Handle at: " << handle() << std::endl ;
    _logger->log<DEBUG5>()  << "-- Type index: " << std::endl ;
    _logger->log<DEBUG5>()  << "---- name: " << typeidx.name() << std::endl ;
    _logger->log<DEBUG5>()  << "---- hash: " << typeidx.hash_code() << std::endl ;
    if ( _logger->wouldWrite<DEBUG5>() ) {
      _logger->log<DEBUG5>() << "-- Geometry dump:" << std::endl ;
      dumpGeometry() ;
    }
    _logger->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
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
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  /**
   *  @brief  EmptyGeometry class
   *  Implement an empty geometry
   */
  class EmptyGeometry : public GeometryPlugin {
  public:
    EmptyGeometry(const EmptyGeometry &) = delete ;
    EmptyGeometry& operator=(const EmptyGeometry &) = delete ;
    
  public:
    EmptyGeometry() : GeometryPlugin( "Empty" ) { /* nop */ }
    
  protected:
    void loadGeometry() { /* nop */ }
    const void *handle() const { return nullptr ; }
    void destroy() { /* nop */ }
    std::type_index typeIndex() const { return std::type_index( typeid(nullptr) ) ; }
    void dumpGeometry() const { /* nop */ }
  };

  MARLIN_DECLARE_GEOPLUGIN( EmptyGeometry )

} // namespace marlin
