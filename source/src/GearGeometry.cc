#include "marlin/GearGeometry.h"

// -- gear headers
#include "gear/GearMgr.h"
#include "gearimpl/Util.h"
#include "gearxml/GearXML.h"

namespace marlin {

  GearGeometry::GearGeometry() :
    GeometryPlugin("Gear") {
    // compact file
    registerParameter( "GearFile",
      "The GEAR geometry XML file",
      _gearFile,
      std::string("")) ;
  }

  //--------------------------------------------------------------------------

  void GearGeometry::loadGeometry() {
    gear::GearXML gearXML( _gearFile ) ;
    _gearMgr.reset( gearXML.createGearMgr() ) ;
  }

  //--------------------------------------------------------------------------

  const void *GearGeometry::handle() const {
    return _gearMgr.get() ;
  }

  //--------------------------------------------------------------------------

  void GearGeometry::destroy() {
    _gearMgr = nullptr ;
  }

  //--------------------------------------------------------------------------

  std::type_index GearGeometry::typeIndex() const {
    return std::type_index( typeid( gear::GearMgr ) ) ;
  }

  //--------------------------------------------------------------------------

  void GearGeometry::dumpGeometry() const {
    _logger->log<DEBUG5>() << *_gearMgr << std::endl ;
  }

} // namespace marlin
