#ifndef MARLIN_GEARGEOMETRY_h
#define MARLIN_GEARGEOMETRY_h 1

// -- marlin headers
#include "marlin/GeometryPlugin.h"

namespace gear {
  class GearMgr ;
}

namespace marlin {
  
  /**
   *  @brief  DD4hepGeometry class
   *  Responsible for loading DD4hep geometry in Marlin 
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

} // end namespace marlin

#endif
