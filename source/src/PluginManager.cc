#include "marlin/concurrency/PluginManager.h"
#include "marlin/concurrency/Internal.h"

// -- std headers
#include <dlfcn.h>
#include <iostream>
#include <cstdlib>
#include <set>

namespace marlin {

  namespace concurrency {

    std::vector<std::string> PluginManager::processorFactoryNames() const {
      std::vector<std::string> names ;
      for ( auto iter : _factories ) {
        names.push_back( iter.first ) ;
      }
      return names ;
    }

    //--------------------------------------------------------------------------

    PluginManager &PluginManager::instance() {
      static PluginManager mgr;
      return mgr;
    }

    //--------------------------------------------------------------------------

    bool PluginManager::loadLibraries( const std::string &envvar )  {
      char *marlinDll = getenv( envvar.c_str() );
      if ( nullptr == marlinDll ) {
        // TODO log a debug message here!
        return true ;
      }
      std::vector<std::string> libraries = StringUtil::split<std::string>( marlinDll , ":" ) ;
      std::set<std::string> checkDuplicateLibs;
      for ( auto library : libraries ) {
        size_t idx = library.find_last_of("/") ;
        // the library basename, i.e. /path/to/libBlah.so --> libBlah.so
        std::string libBaseName( library.substr( idx + 1 ) );
        auto inserted = checkDuplicateLibs.insert( libBaseName ).second ;
        if ( not inserted ) {
          std::cerr << std::endl << "<!-- ERROR loading shared library : " << library << std::endl
              << "    ->    Trying to load DUPLICATE library -->" << std::endl << std::endl ;
          return false ;
        }
        void* libPointer = dlopen( library.c_str() , RTLD_LAZY | RTLD_GLOBAL) ;
        if( nullptr == libPointer ) {
          std::cerr << std::endl << "<!-- ERROR loading shared library : " << library << std::endl
                      << "    ->    "   << dlerror() << " -->" << std::endl << std::endl ;
          return false ;
        }
        _libraries.push_back( libPointer ) ;
      }
      return true ;
    }

    //--------------------------------------------------------------------------

    std::shared_ptr<Processor> PluginManager::createProcessor( const std::string &type, const std::string &name ) {
      auto iter = _factories.find( type ) ;
      if ( _factories.end() == iter ) {
        return nullptr;
      }
      return iter->second->create( name );
    }

    //--------------------------------------------------------------------------

    void PluginManager::dump() const {
      std::cout << "------------------------------------" << std::endl ;
      std::cout << " ** MarlinMT plugin manager dump ** " << std::endl ;
      if ( _factories.empty() ) {
        std::cout << " No processor registered ..." << std::endl ;
      }
      else {
        std::cout << " + Registered processors: " << std::endl ;
        for ( auto iter : _factories ) {
          std::cout << "   - " << iter.first << std::endl ;
        }
      }
      std::cout << "----------------------------------" << std::endl ;
    }

  } // namespace concurrency

} // namespace marlin
