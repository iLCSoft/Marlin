#ifndef MARLIN_CONCURRENCY_PLUGINMANAGER_h
#define MARLIN_CONCURRENCY_PLUGINMANAGER_h 1

// -- std headers
#include <map>
#include <string>

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/concurrency/ProcessorFactory.h"

// registration macros
#define MARLIN_REGISTER_PROCESSOR_NAME( Class, Name ) \
  { PluginManager::instance().registerProcessorFactory<Class>( Name ) ; }

#define MARLIN_REGISTER_PROCESSOR( Class ) \
  MARLIN_REGISTER_PROCESSOR_NAME( Class, #Class )

namespace marlin {

  namespace concurrency {

    /**
     *  @brief  PluginManager singleton class
     *  Responsible for loading shared libraries and collecting
     *  processor factory instances. Processor instances can be
     *  created from factories using the PluginManager::create()
     *  method on query.
     */
    class PluginManager {
    public:
      // traits
      typedef std::shared_ptr<ProcessorFactory>           FactoryType ;
      typedef std::map<const std::string, FactoryType>    FactoryMap ;
      typedef std::vector<void*>                          LibraryList ;

    private:
      PluginManager(const PluginManager &) = delete ;
      PluginManager& operator=(const PluginManager &) = delete ;
      PluginManager() = default ;
      ~PluginManager() = default ;

      /**
       *  @brief  Register a new processor factory
       *  The template parameter is the processor type
       *
       *  @param  name the factory
       */
      template <typename T>
      void registerProcessorFactory( const std::string &name ) ;

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
      std::vector<std::string> factoryNames() const ;

      /**
       *  @brief  Create a processor instance
       *
       *  @param  type the processor type
       *  @param  name the processor name
       */
      std::shared_ptr<Processor> create( const std::string &type, const std::string &name );

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

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    template <typename T>
    inline void PluginManager::registerProcessorFactory( const std::string &name ) {
      auto iter = _factories.find( name ) ;
      if ( _factories.end() != iter ) {
        throw Exception( "PluginManager::registerFactory: factory '" + name + "' already registered!" );
      }
      FactoryType factory = std::make_shared<ProcessorFactoryT<T>>() ;
      _factories.insert( FactoryMap::value_type( name, factory ) );
    }

  } // end namespace concurrency

} // end namespace marlin

#endif
