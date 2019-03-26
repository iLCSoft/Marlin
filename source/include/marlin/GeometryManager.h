#ifndef MARLIN_GEOMETRYMANAGER_h
#define MARLIN_GEOMETRYMANAGER_h 1

// -- std headers
#include <typeindex>

// -- marlin headers
#include <marlin/Logging.h>
#include <marlin/Exceptions.h>
#include <marlin/GeometryPlugin.h>

namespace marlin {

  class Application ;

  /**
   *  @brief  GeometryManager class.
   *  Handle a user plugin in charge of loading the geometry
   *  in the framework and providing access to it.
   */
  class GeometryManager {
  public:
    using Logger = Logging::Logger ;

  public:
    GeometryManager(const GeometryManager &) = delete ;
    GeometryManager& operator=(const GeometryManager &) = delete ;
    ~GeometryManager() = default ;

    /**
     *  @brief  Default constructor
     */
    GeometryManager() ;

    /**
     *  @brief  Initialize the geometry manager.
     *  Load the geometry plugin service and create the geometry
     *
     *  @param  app the application from which to get settings
     */
    void init( const Application *app ) ;

    /**
     *  @brief  Get the underlying geometry handle
     *  Example:
     *
     *  @code{cpp}
     *  // assume Gear geometry has been loaded
     *  const gear::GearMgr* gearGeo = mgr->geometry<gear::GearMgr>() ;
     *  // assume DD4hep geometry has been loaded
     *  const dd4hep::Detector* dd4hepGeo = mgr->geometry<dd4hep::Detector>() ;
     *  @endcode
     */
    template <typename T>
    const T *geometry() const ;

    /**
     *  @brief  Get the underlying geometry type info
     */
    std::type_index typeIndex() const ;

    /**
     *  @brief  Clear the geometry content
     */
    void clear() ;

    /**
     *  @brief  Whether the geometry has been initialized
     */
    bool isInitialized() const ;

    /**
     *  @brief  Get the application in which the manager is running
     *  Valid only after initialization
     */
    const Application &app() const ;

  private:
    /// The geometry plugin created on init()
    std::unique_ptr<GeometryPlugin>      _plugin {nullptr} ;
    /// The application in which the geometry manager has been initialized
    const Application                   *_application {nullptr} ;
    /// The logger instance
    Logger                               _logger {nullptr} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename T>
  inline const T *GeometryManager::geometry() const {
    if ( nullptr == _plugin ) {
      throw Exception( "GeometryManager::geometry: geometry plugin not created !" ) ;
    }
    auto ptr = _plugin->handle() ;
    if ( nullptr == ptr ) {
      return nullptr ;
    }
    const T *castHandle = static_cast<const T*>( ptr ) ;
    if ( nullptr == castHandle ) {
      throw Exception( "GeometryManager::geometry: invalid geometry cast !" ) ;
    }
    return castHandle ;
  }

} // end namespace marlin

#endif
