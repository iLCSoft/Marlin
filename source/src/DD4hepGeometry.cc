#include "marlin/DD4hepGeometry.h"

// -- marlin headers
#include "marlin/PluginManager.h"

// -- dd4hep headers
#include "DD4hep/Detector.h"
#include "DD4hep/DetType.h"
#include "DD4hep/DetectorSelector.h"
#include "DD4hep/DD4hepUnits.h"
#include "DDRec/Surface.h"
#include "DDRec/DetectorSurfaces.h"
#include "DDRec/DetectorData.h"
#include "DDRec/SurfaceHelper.h"

using namespace dd4hep ;
using namespace dd4hep::detail ;
using namespace dd4hep::rec ;

namespace marlin {

  DD4hepGeometry::DD4hepGeometry() :
    GeometryPlugin("DD4hep") {
      _description = "DD4hep geometry plugin. Use geoMgr->geometry<dd4hep::Detector>() to access the geometry" ;
    // compact file
    registerParameter( "CompactFile",
      "The DD4hep geometry compact XML file",
      _compactFile,
      std::string("")) ;
    // dump detector data ?
    registerOptionalParameter( "DumpDetectorData",
      "Whether to dump detector data while dumping the geometry",
      _dumpDetectorData,
      false) ;
    // dump detector surfaces ?
    registerOptionalParameter( "DumpDetectorSurfaces",
      "Whether to dump surfaces while dumping the geometry",
      _dumpDetectorSurfaces,
      false) ;
  }

  //--------------------------------------------------------------------------

  void DD4hepGeometry::loadGeometry() {
    dd4hep::Detector& theDetector = dd4hep::Detector::getInstance() ;
    theDetector.fromCompact( _compactFile ) ;
  }

  //--------------------------------------------------------------------------

  const void *DD4hepGeometry::handle() const {
    return &dd4hep::Detector::getInstance() ;
  }

  //--------------------------------------------------------------------------

  void DD4hepGeometry::destroy() {
    dd4hep::Detector::getInstance().destroyInstance() ;
  }

  //--------------------------------------------------------------------------

  std::type_index DD4hepGeometry::typeIndex() const {
    return std::type_index( typeid( dd4hep::Detector ) ) ;
  }

  //--------------------------------------------------------------------------

  void DD4hepGeometry::printDetectorData( DetElement det ) const {
    try {
      FixedPadSizeTPCData* d = det.extension<FixedPadSizeTPCData>() ;
      _logger->log<DEBUG5>() << *d ;
    }
    catch(...){}
    try {
      ZPlanarData* d = det.extension<ZPlanarData>() ;
      _logger->log<DEBUG5>() << *d ;
    }
    catch(...){}
    try {
      ZDiskPetalsData* d = det.extension<ZDiskPetalsData>() ;
      _logger->log<DEBUG5>() << *d ;
    }
    catch(...){}
    try {
      ConicalSupportData* d = det.extension<ConicalSupportData>() ;
      _logger->log<DEBUG5>() << *d ;
    }
    catch(...){}
    try {
      LayeredCalorimeterData* d = det.extension<LayeredCalorimeterData>() ;
      _logger->log<DEBUG5>() << *d ;
    }
    catch(...){}
  }

  //--------------------------------------------------------------------------

  void DD4hepGeometry::printDetectorSets( const std::string &name, unsigned int includeFlag,  unsigned int excludeFlag ) const {
    Detector& description = Detector::getInstance();
    const std::vector<DetElement>& dets = DetectorSelector(description).detectors( includeFlag, excludeFlag ) ;
    _logger->log<DEBUG5>() << " " << name  ;
    for(int i=0,N=dets.size();i<N;++i) {
      _logger->log<DEBUG5>() << dets[i].name() << ", " ;
    }
    _logger->log<DEBUG5>() << std::endl ;
  }

  //--------------------------------------------------------------------------

  void DD4hepGeometry::dumpGeometry() const {
    Detector& description = Detector::getInstance();
    _logger->log<DEBUG5>() << "############################################################################### "  << std::endl  ;
    Header h = description.header() ;
    _logger->log<DEBUG5>()
        << " detector model : " <<  h.name()  << std::endl
  	    << "    title : "  << h.title() << std::endl
  	    << "    author : " << h.author() << std::endl
  	    << "    status : " << h.status() << std::endl ;
    // print a few sets of detectors (mainly to demonstrate the usage of the detector types )
    printDetectorSets( " barrel trackers : " , ( DetType::TRACKER | DetType::BARREL ) , ( DetType::VERTEX) ) ;
    printDetectorSets( " endcap trackers : " , ( DetType::TRACKER | DetType::ENDCAP ) , ( DetType::VERTEX) ) ;
    printDetectorSets( " vertex barrel trackers : " , ( DetType::TRACKER | DetType::BARREL | DetType::VERTEX), DetType::IGNORE ) ;
    printDetectorSets( " vertex endcap trackers : " , ( DetType::TRACKER | DetType::ENDCAP | DetType::VERTEX), DetType::IGNORE ) ;
    printDetectorSets( " barrel calorimeters : " , ( DetType::CALORIMETER | DetType::BARREL ), DetType::IGNORE ) ;
    printDetectorSets( " endcap calorimeters : " , ( DetType::CALORIMETER | DetType::ENDCAP ), DetType::IGNORE ) ;
    // everything that is not TRACKER or CALORIMETER
    printDetectorSets( " other detecors : " , ( DetType::IGNORE ) , ( DetType::CALORIMETER | DetType::TRACKER ) ) ;
    // dump detector data
    if( _dumpDetectorData ) {
      _logger->log<DEBUG5>() << "############################################################################### "  << std::endl  ;
      dd4hep::Detector::HandleMap dets = description.detectors() ;
      for( dd4hep::Detector::HandleMap::const_iterator it = dets.begin() ; it != dets.end() ; ++it ) {
        DetElement det = it->second ;
        _logger->log<DEBUG5>() << " ---------------------------- " << det.name() << " ----------------------------- " << std::endl ;
        DetType type( det.typeFlag() ) ;
        _logger->log<DEBUG5>() << " ------     " << type << std:: endl ;
        printDetectorData( det ) ;
      }
      _logger->log<DEBUG5>() << "############################################################################### "  << std::endl  ;
    }
    dd4hep::Detector::HandleMap sensDet = description.sensitiveDetectors() ;
    _logger->log<DEBUG5>() << "############################################################################### "  << std::endl ;
    _logger->log<DEBUG5>() << "     sensitive detectors:     " << std::endl ;
    for( dd4hep::Detector::HandleMap::const_iterator it = sensDet.begin() ; it != sensDet.end() ; ++it ){
      SensitiveDetector sDet = it->second ;
      _logger->log<DEBUG5>() << "     " << it->first << " : type = " << sDet.type() << std::endl ;
    }
    std::cout << "############################################################################### "  << std::endl  << std::endl  ;
  	DetElement world = description.world() ;
    //------------------ breadth first tree traversal ---------
    std::list< DetElement > dets ;
    std::list< DetElement > daugs ;
    std::list< DetElement > gdaugs ;
    daugs.push_back( world ) ;
    while( ! daugs.empty() ) {
      for( std::list< DetElement >::iterator li=daugs.begin() ; li != daugs.end() ; ++li ){
        DetElement dau = *li ;
        DetElement::Children chMap = dau.children() ;
        for ( DetElement::Children::const_iterator it=chMap.begin() ; it != chMap.end() ; ++it ){
          DetElement de = (*it).second ;
          gdaugs.push_back( de ) ;
        }
      }
      dets.splice( dets.end() , daugs ) ;
      daugs.splice( daugs.end() , gdaugs ) ;
    }
    //------------------ end tree traversal ---------


    for ( std::list< DetElement >::const_iterator it=dets.begin() ; it != dets.end() ; ++it ) {
      DetElement de = (*it) ;
      DetElement mother = de.parent() ;
      unsigned parentCount = 0 ;
      while( mother.isValid() ) {
        mother = mother.parent() ;
        ++parentCount ;
      }
      SurfaceHelper surfMan( de ) ;
      const SurfaceList& sL = surfMan.surfaceList() ;
      _logger->log<DEBUG5>() << "DetElement: " ;
      for(unsigned i=0 ; i < parentCount ; ++i ) {
        _logger->log<DEBUG5>() << "\t" ;
      }
      _logger->log<DEBUG5>()
        << de.name()
        << "[ path: "<< de.placementPath ()  <<  "] (id: " << de.id() << ") - sens type : "
        << description.sensitiveDetector( de.name() ).type()
        << "\t surfaces : " <<  ( sL.empty() ? 0 : sL.size()  )
        << std::endl ;
      if( _dumpDetectorSurfaces ) {
        for( SurfaceList::const_iterator sit = sL.begin() ; sit != sL.end() ; ++sit ) {
        	const ISurface* surf =  *sit ;
        	_logger->log<DEBUG5>() << " ------------------------- "
        		  << " surface: "  << *surf         << std::endl
        		  << " ------------------------- "  << std::endl ;
        }
      }
    }
    _logger->log<DEBUG5>() << "############################################################################### "  << std::endl  << std::endl  ;
  }
  
  MARLIN_DECLARE_GEOPLUGIN( DD4hepGeometry )

} // namespace marlin
