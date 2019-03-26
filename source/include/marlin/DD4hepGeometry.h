#ifndef MARLIN_DD4HEPGEOMETRY_h
#define MARLIN_DD4HEPGEOMETRY_h 1

// -- marlin headers
#include "marlin/GeometryPlugin.h"

namespace dd4hep {
  class DetElement ;
}

namespace marlin {
  
  /**
   *  @brief  DD4hepGeometry class
   *  Responsible for loading DD4hep geometry in Marlin 
   */
  class DD4hepGeometry : public GeometryPlugin {
  public:
    DD4hepGeometry(const DD4hepGeometry &) = delete ;
    DD4hepGeometry& operator=(const DD4hepGeometry &) = delete ;
    
  public:
    DD4hepGeometry() ;
    
  protected:
    void loadGeometry() ;
    const void *handle() const ;
    void destroy() ;
    std::type_index typeIndex() const ;
    void dumpGeometry() const ;
    
  private:
    void printDetectorData( dd4hep::DetElement det ) const ;
    void printDetectorSets( const std::string &name, unsigned int includeFlag, unsigned int excludeFlag ) const ;
    
  private:
    /// The DD4hep geometry compact XML file
    std::string         _compactFile {} ;
    /// Whether to dump detector data while dumping the geometry
    bool                _dumpDetectorData {false} ;
    /// Whether to dump surfaces while dumping the geometry
    bool                _dumpDetectorSurfaces {false} ;
  };

} // end namespace marlin

#endif
