#include "marlin/GeometryManager.h"

// -- marlin headers
#include "marlin/PluginManager.h"
#include "marlin/Application.h"

namespace marlin {

  GeometryManager::GeometryManager() {
    _logger = Logging::createLogger( "GeometryManager" ) ;
  }

  //--------------------------------------------------------------------------

  void GeometryManager::init( const Application *application ) {
    if ( isInitialized() ) {
      throw Exception( "GeometryManager::init: already initialized !" ) ;
    }
    _application = application ;
    _logger = app().createLogger( "GeometryManager" ) ;
    auto &mgr = PluginManager::instance() ;
    auto geometryParameters = app().geometryParameters() ;
    if ( nullptr == geometryParameters ) {
      _logger->log<WARNING>() << "No geometry section found. Creating empty geometry" << std::endl ;
      _plugin = mgr.create<GeometryPlugin>( PluginType::GeometryPlugin, "EmptyGeometry" ) ;
    }
    else {
      auto geometryType = geometryParameters->getStringVal( "GeometryType" ) ;
      _plugin = mgr.create<GeometryPlugin>( PluginType::GeometryPlugin, geometryType ) ;
      if ( nullptr == _plugin ) {
        throw Exception( "GeometryManager::init: Couldn't find geometry plugin '" + geometryType + "'..." ) ;
      }
      _plugin->setParameters( geometryParameters ) ;
    }
    _plugin->init( _application ) ;
    _plugin->print() ;
  }

  //--------------------------------------------------------------------------

  std::type_index GeometryManager::typeIndex() const {
    if ( nullptr == _plugin ) {
      throw Exception( "GeometryManager::typeIndex: geometry not initialized !" ) ;
    }
    return _plugin->typeIndex() ;
  }

  //--------------------------------------------------------------------------

  void GeometryManager::clear() {
    if ( nullptr != _plugin ) {
      _plugin->destroy() ;
      _plugin = nullptr ;
    }
    _application = nullptr ;
  }

  //--------------------------------------------------------------------------

  bool GeometryManager::isInitialized() const {
    return ( nullptr != _application && nullptr != _plugin ) ;
  }

  //--------------------------------------------------------------------------

  const Application &GeometryManager::app() const {
    if ( nullptr == _application ) {
      throw Exception( "GeometryManager::app: not initilialized !" ) ;
    }
    return *_application ;
  }

} // namespace marlin
