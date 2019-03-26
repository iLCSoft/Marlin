#include "marlin/GeometryManager.h"

namespace marlin {

  GeometryManager::GeometryManager() {
    _logger = Logging::createLogger( "Geometry" ) ;
  }

  //--------------------------------------------------------------------------

  void GeometryManager::init( const Application *application ) {
    if ( isInitialized() ) {
      throw Exception( "GeometryManager::init: already initialized !" ) ;
    }
    _application = application ;
    // _logger = _application->logger() ;
    // TODO investigate how to create a geometry plugin nicely:
    // - plugin inheritance
    // - plugin manager interface
    // - settings loading
    // _plugin = createNicelyMyPlugin() ;
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
