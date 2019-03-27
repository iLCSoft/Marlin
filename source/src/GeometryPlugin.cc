#include "marlin/GeometryPlugin.h"

// -- marlin headers
#include "marlin/VerbosityLevels.h"
#include "marlin/PluginManager.h"

namespace marlin {

  GeometryPlugin::GeometryPlugin( const std::string &gtype ) :
    _type(gtype) {
    // create a standalone logger
    _logger = Logging::createLogger( "Geometry" ) ;
    // Whether to dump the geometry on creation
    registerOptionalParameter( "DumpGeometry",
      "Whether to dump the geometry on creation",
      _dumpGeometry,
      false) ;
  }

  //--------------------------------------------------------------------------

  void GeometryPlugin::print() const {
    auto typeidx = typeIndex() ;
    _logger->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
    _logger->log<MESSAGE>() << "-- Geometry plugin: " << type() << std::endl ;
    _logger->log<MESSAGE>() << "-- Description: " << description() << std::endl ;
    _logger->log<MESSAGE>() << "-- Handle at: " << handle() << std::endl ;
    _logger->log<MESSAGE>()  << "-- Type index: " << std::endl ;
    _logger->log<MESSAGE>()  << "---- name: " << typeidx.name() << std::endl ;
    _logger->log<MESSAGE>()  << "---- hash: " << typeidx.hash_code() << std::endl ;
    if ( _dumpGeometry ) {
      _logger->log<MESSAGE>() << "-- Geometry dump:" << std::endl ;
      dumpGeometry() ;
      _logger->log<MESSAGE>() << "-- End of geometry dump" << std::endl ;
    }
    _logger->log<MESSAGE>() << "----------------------------------------------------------" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void GeometryPlugin::init( const Application *application ) {
    _application = application ;
    loadGeometry() ;
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
