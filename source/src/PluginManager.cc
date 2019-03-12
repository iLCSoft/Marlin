#include "marlin/PluginManager.h"

// -- std headers
#include <dlfcn.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <set>

// -- lcio headers
#include <UTIL/BitField64.h>  // LCTokenizer

namespace marlin {
  
  void PluginManager::registerProcessorFactory( Processor *proc ) {
    auto iter = _factories.find( proc->type() ) ;
    if ( _factories.end() != iter ) {
      throw Exception( "PluginManager::registerFactory: factory '" + proc->type() + "' already registered!" );
    }
    _factories.insert( FactoryMap::value_type( proc->name(), proc ) ) ;
  }
  
  //--------------------------------------------------------------------------

  std::vector<std::string> PluginManager::processorTypes() const {
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
    std::string marlinDllStr (marlinDll) ;    
    std::vector<std::string> libraries ;
    LCTokenizer tokenizer ( libraries , ':' ) ;
    std::for_each( marlinDllStr.begin() , marlinDllStr.end() , tokenizer ) ;
    
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

  Processor* PluginManager::createProcessor( const std::string &type, const std::string &name ) {
    auto iter = _factories.find( type ) ;
    if ( _factories.end() == iter ) {
      return nullptr;
    }
    auto proc = iter->second->newProcessor() ;
    proc->setName( name ) ;
    return proc ;
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

} // namespace marlin
