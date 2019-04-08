
// -- marlin headers
#include "marlin/GeometryPlugin.h"
#include "marlin/PluginManager.h"

// -- gear headers
#include "gear/GearMgr.h"
#include "gearimpl/Util.h"
#include "gearxml/GearXML.h"

namespace marlin {
  
  /**
   *  @brief  GearGeometry class
   *  Responsible for loading Gear geometry in Marlin 
   */
  class GearGeometry : public GeometryPlugin {
  public:
    GearGeometry(const GearGeometry &) = delete ;
    GearGeometry& operator=(const GearGeometry &) = delete ;
    
  public:
    GearGeometry() ;
    
  protected:
    void loadGeometry() ;
    const void *handle() const ;
    void destroy() ;
    std::type_index typeIndex() const ;
    void dumpGeometry() const ;
    
  private:
    /// The GEAR geometry XML file
    std::string                       _gearFile {} ;
    /// The Gear manager handling geometry
    std::unique_ptr<gear::GearMgr>    _gearMgr {nullptr} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  GearGeometry::GearGeometry() :
    GeometryPlugin("Gear") {
    _description = "Gear geometry plugin. Use geoMgr->geometry<gear::GearMgr>() to access the geometry" ;
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
    _logger->log<MESSAGE>() << *_gearMgr << std::endl ;
  }

  MARLIN_DECLARE_GEOPLUGIN( GearGeometry )
  
} // namespace marlin

