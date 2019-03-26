#ifndef MARLIN_GEOMETRYPLUGIN_h
#define MARLIN_GEOMETRYPLUGIN_h 1

// -- std headers
#include <map>
#include <string>

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/Application.h"
#include "marlin/Parametrized.h"

namespace marlin {
  
  class GeometryManager ;

  /**
   *  @brief  GeometryPlugin class
   *  Responsible for loading geometry in Marlin and providing 
   *  access to it through the GeometryManager 
   */
  class GeometryPlugin : public Parametrized {
    friend class GeometryManager ;
    
  public:
    using logger_ptr = Application::logger_ptr;
    
  public:
    GeometryPlugin() = delete ;
    virtual ~GeometryPlugin() = default ;
    GeometryPlugin(const GeometryPlugin &) = delete ;
    GeometryPlugin& operator=(const GeometryPlugin &) = delete ;
    
  protected:
    /**
     *  @brief  Constructor. To be called by sub classes
     * 
     *  @param  gtype the geometry type
     */
    GeometryPlugin( const std::string &gtype ) ;
    
  protected:
    /**
     *  @brief  Load the geometry
     */
    virtual void loadGeometry() = 0 ;
    
    /**
     *  @brief  Get a handle on the geometry instance
     */
    virtual const void *handle() const = 0 ;

    /**
     *  @brief  Cleanup geometry
     */
    virtual void destroy() = 0 ;
    
  public:
    /**
     *  @brief  Get a type index object from the geometry handle
     */
    virtual std::type_index typeIndex() const = 0 ;
    
    /**
     *  @brief  Dump the geometry in the console
     */
    virtual void dumpGeometry() const = 0 ;
    
    /**
     *  @brief  Get the geometry description.
     *  Can be set by sub-classes in constructor (protected)
     */
    const std::string &description() const ;
    
    /**
     *  @brief  Get the geometry type
     */
    const std::string &type() const ;
    
    /**
     *  Print the complete geometry plugin description.
     *  The geometry is dumped if the verbosity level is less than DEBUG5
     */
    void print() const ;
    
  protected:
    /**
     *  @brief  Get the application in which the plugin has been created
     */
    const Application &app() const ;
    
  private:
    /**
     *  @brief  Initialize the geometry plugin. 
     *  Called by the geometry manager at startup 
     *  
     *  @param  app the application from which to get settings
     */
    void init( const Application *app ) ;
    
  protected:
    /// The geometry type
    const std::string   _type ;
    /// The geometry plugin description
    std::string         _description {"No description"} ;
    /// The application logger
    logger_ptr          _logger {nullptr} ;
    /// The verbosity level
    std::string         _verbosity {} ;

  private:
    /// The Marlin application
    const Application  *_application {nullptr} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  inline const std::string &GeometryPlugin::description() const { 
    return _description ; 
  }
  
  //--------------------------------------------------------------------------
  
  inline const std::string &GeometryPlugin::type() const {
    return _type ;
  }

} // end namespace marlin

#endif
