#ifndef MARLIN_PLUGINMANAGER_h
#define MARLIN_PLUGINMANAGER_h 1

// -- std headers
#include <map>
#include <string>

// -- marlin headers
#include "marlin/Exceptions.h"

namespace marlin {

  /**
   *  @brief  PluginManager singleton class
   *  Responsible for loading shared libraries and collecting
   *  processor factory instances. Processor instances can be
   *  created from factories using the PluginManager::create()
   *  method on query.
   */
  class PluginManager {
    // TODO find a better mechanism for plugins
    friend class Processor ; // for registration
    
  public:
    // typedefs
    typedef Processor*                                  FactoryType ;
    typedef std::map<const std::string, FactoryType>    FactoryMap ;
    typedef std::vector<void*>                          LibraryList ;

  private:
    PluginManager(const PluginManager &) = delete ;
    PluginManager& operator=(const PluginManager &) = delete ;
    PluginManager() = default ;
    ~PluginManager() = default ;
    
    /**
     *  @brief  Register a new processor factory
     *
     *  @param  proc the prototype processor used as factory
     */
    void registerProcessorFactory( Processor *proc ) ;

  public:
    /**
     *  @brief  Get the plugin manager instance
     */
    static PluginManager &instance() ;

    /**
     *  @brief  Load shared libraries to populate the list of plugins
     *
     *  @param  envvar the environment variable to load the libraries from
     */
    bool loadLibraries( const std::string &envvar = "MARLIN_DLL" ) ;

    /**
     *  @brief  Get the list of registered processor factories
     *  The list of returned names corresponds to the processor types
     */
    std::vector<std::string> processorTypes() const ;

    /**
     *  @brief  Create a new processor instance.
     *  Ownership transfered to the called
     *
     *  @param  type the processor type
     *  @param  name the processor name
     */
    Processor* createProcessor( const std::string &type, const std::string &name ) ;

    /**
     *  @brief  Dump plugin manager content in console
     */
    void dump() const ;

  private:
    /// The map of processor factories
    FactoryMap              _factories {} ;
    /// The list of loaded libraries
    LibraryList             _libraries {} ;
  };

} // end namespace marlin

#endif
