#ifndef MARLIN_PLUGINMANAGER_h
#define MARLIN_PLUGINMANAGER_h 1

// -- std headers
#include <map>
#include <string>
#include <mutex>
#include <memory>
#include <vector>
#include <functional>

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/Logging.h"

// Helper macros to declare plugins
// R. Ete: Impossible to make a function outside of main.
// The workaround is to create a structure on-the-fly that make the function call in the constructor
#define MARLIN_DECLARE_PLUGIN( Class, NameStr, Type ) \
  namespace marlin_plugins { \
    struct PluginDeclaration_##Class { \
      PluginDeclaration_##Class() { \
        marlin::PluginManager::instance().registerPlugin<Class>( Type, NameStr, false ) ; \
      } \
    }; \
    static PluginDeclaration_##Class __instance_##Class ; \
  }
// geometry plugin declaration
#define MARLIN_DECLARE_GEOPLUGIN_NAME( Class, NameStr ) MARLIN_DECLARE_PLUGIN( Class, NameStr, marlin::PluginType::GeometryPlugin )
#define MARLIN_DECLARE_GEOPLUGIN( Class ) MARLIN_DECLARE_GEOPLUGIN_NAME( Class, #Class )
// processor plugin declaration
#define MARLIN_DECLARE_PROCESSOR_NAME( Class, NameStr ) MARLIN_DECLARE_PLUGIN( Class, NameStr, marlin::PluginType::Processor )
#define MARLIN_DECLARE_PROCESSOR( Class ) MARLIN_DECLARE_PROCESSOR_NAME( Class, #Class )
// data source plugin declaration
#define MARLIN_DECLARE_DATASOURCE_NAME( Class, NameStr ) MARLIN_DECLARE_PLUGIN( Class, NameStr, marlin::PluginType::DataSource )
#define MARLIN_DECLARE_DATASOURCE( Class ) MARLIN_DECLARE_DATASOURCE_NAME( Class, #Class )

namespace marlin {

  /**
   *  @brief  PluginType enumerator
   *  Enumerate only possible Marlin plugin types.
   *  Might be extended with framework development.
   */
  enum class PluginType : int {
    Processor,
    GeometryPlugin,
    DataSource
  } ;

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  /**
   *  @brief  PluginManager singleton class
   *  Responsible for loading shared libraries and collecting
   *  processor factory instances. Processor instances can be
   *  created from factories using the PluginManager::create()
   *  method on query.
   */
  class PluginManager {
  public:
    // typedefs
    typedef std::shared_ptr<void>                       PluginPtr ;
    typedef std::function<PluginPtr()>                  FactoryFunction ;
    typedef std::map<std::string, FactoryFunction>      FactoryMap ;
    typedef std::map<PluginType, FactoryMap>            PluginFactoryMap ;
    typedef std::vector<void*>                          LibraryList ;
    typedef Logging::Logger                             Logger ;
    typedef std::recursive_mutex                        mutex_type ;
    typedef std::lock_guard<mutex_type>                 lock_type ;

  private:
    PluginManager(const PluginManager &) = delete ;
    PluginManager& operator=(const PluginManager &) = delete ;
    ~PluginManager() = default ;

    /**
     *  @brief  Constructor
     */
    PluginManager() ;

  public:
    /**
     *  @brief  Get the plugin manager instance
     */
    static PluginManager &instance() ;

    /**
     *  @brief  Register a new plugin to the manager.
     *  A new factory function creating an object of type T
     *  is inserted into the registry. The type T must be
     *  default constructible. If you want to provide a custom
     *  factory function, use the corresponding overloaded function.
     *  If the flag ignoreDuplicate is set to true, no exception is
     *  thrown in case a duplicate is found in the registry. In this
     *  case, the registry is not modified.
     *
     *  @param  type the plugin type
     *  @param  name the plugin name
     *  @param  ignoreDuplicate whether to avoid exception throw in case of duplicate entry
     */
    template <typename T>
    void registerPlugin( PluginType type, const std::string &name, bool ignoreDuplicate = true ) ;

    /**
     *  @brief  Register a new plugin to the manager.
     *  See overloaded function description for more details
     *
     *  @param  type the plugin type
     *  @param  name the plugin name
     *  @param  factoryFunction the factory function responsible for the plugin creation
     *  @param  ignoreDuplicate whether to avoid exception throw in case of duplicate entry
     */
    void registerPlugin( PluginType type, const std::string &name,
      FactoryFunction factoryFunction, bool ignoreDuplicate = true ) ;

    /**
     *  @brief  Load shared libraries to populate the list of plugins
     *
     *  @param  envvar the environment variable to load the libraries from
     */
    bool loadLibraries( const std::string &envvar = "MARLIN_DLL" ) ;

    /**
     *  @brief  Get all registered plugin name for the given type
     *
     *  @param  type the plugin type
     */
    std::vector<std::string> pluginNames( PluginType type ) const ;

    /**
     *  @brief  Whether the plugin of given type and name is registered
     *
     *  @param  type the plugin type to check
     *  @param  name the plugin name to check
     */
    bool pluginRegistered( PluginType type, const std::string &name ) const ;

    /**
     *  @brief  Create a new plugin instance.
     *  A factory function must have been registered before hand.
     *  The template parameter T is the final plugin type requested
     *  by the caller.
     *
     *  @param  type the plugin type
     *  @param  name the plugin name
     */
    template <typename T>
    std::shared_ptr<T> create( PluginType type, const std::string &name ) const ;

    /**
     *  @brief  Dump plugin manager content in console
     */
    void dump() const ;

    /**
     *  @brief  Get the plugin manager logger
     */
    Logger logger() const ;

  private:
    /**
     *  @brief  Convert plugin type to string
     *
     *  @param  type plugin type from enumerator
     */
    static std::string pluginTypeToString( PluginType type ) ;

  private:
    /// The map of plugin factories
    PluginFactoryMap           _pluginFactories {} ;
    /// The list of loaded libraries
    LibraryList                _libraries {} ;
    /// The plugin manager logger
    mutable Logger             _logger {nullptr} ;
    /// The synchronization mutex
    mutable mutex_type         _mutex {} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename T>
  inline void PluginManager::registerPlugin( PluginType type, const std::string &name,
    bool ignoreDuplicate ) {
    FactoryFunction factoryFunction = [](){
      return std::make_shared<T>() ;
    };
    registerPlugin( type, name, factoryFunction, ignoreDuplicate ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::shared_ptr<T> PluginManager::create( PluginType type, const std::string &name ) const {
    lock_type lock( _mutex ) ;
    auto typeIter = _pluginFactories.find( type ) ;
    auto factoryIter = typeIter->second.find( name ) ;
    // plugin not found ?
    if ( typeIter->second.end() == factoryIter ) {
      auto typeStr = pluginTypeToString( type ) ;
      _logger->log<DEBUG5>() << "Plugin not found: type '" << typeStr << "', name '" << name << "'" << std::endl ;
      return nullptr ;
    }
    auto pointer = factoryIter->second() ; // factory function call
    return std::static_pointer_cast<T, void>( pointer ) ;
  }

} // end namespace marlin

#endif
