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

  PluginManager::PluginManager() {
    _pluginFactories.insert( PluginFactoryMap::value_type(PluginType::Processor, FactoryMap()) ) ;
    _pluginFactories.insert( PluginFactoryMap::value_type(PluginType::GeometryPlugin, FactoryMap()) ) ;
    _pluginFactories.insert( PluginFactoryMap::value_type(PluginType::DataSource, FactoryMap()) ) ;
    _logger = Logging::createLogger( "PluginManager" ) ;
    _logger->setLevel<MESSAGE>() ;
  }

  //--------------------------------------------------------------------------

  void PluginManager::registerPlugin( PluginType type, const std::string &name,
    FactoryFunction factoryFunction, bool ignoreDuplicate ) {
    lock_type lock( _mutex ) ;
    auto typeIter = _pluginFactories.find( type ) ;
    auto factoryIter = typeIter->second.find( name ) ;
    if ( typeIter->second.end() != factoryIter ) {
      if ( not ignoreDuplicate ) {
        _logger->log<ERROR>() << "PluginManager::registerPlugin: plugin '" << name << "' already registered!" << std::endl ;
        throw Exception( "PluginManager::registerPlugin: plugin '" + name + "' already registered!" ) ;
      }
      _logger->log<DEBUG2>() << "PluginManager::registerPlugin: plugin '" << name << "' already registered! Skipping ..." << std::endl ;
    }
    else {
      typeIter->second.insert( FactoryMap::value_type( name, factoryFunction ) ) ;
      auto typeStr = pluginTypeToString( type ) ;
      _logger->log<DEBUG5>() << "New plugin registered: type '" << typeStr << "', name '" << name << "'" <<std::endl ;
    }
  }

  //--------------------------------------------------------------------------

  std::vector<std::string> PluginManager::pluginNames( PluginType type ) const {
    lock_type lock( _mutex ) ;
    std::vector<std::string> names ;
    auto typeIter = _pluginFactories.find( type ) ;
    for ( auto iter : typeIter->second ) {
      names.push_back( iter.first ) ;
    }
    return names ;
  }

  //--------------------------------------------------------------------------

  bool PluginManager::pluginRegistered( PluginType type, const std::string &name ) const {
    lock_type lock( _mutex ) ;
    auto typeIter = _pluginFactories.find( type ) ;
    return ( typeIter->second.find( name ) != typeIter->second.end() ) ;
  }

  //--------------------------------------------------------------------------

  PluginManager &PluginManager::instance() {
    static PluginManager mgr;
    return mgr;
  }

  //--------------------------------------------------------------------------

  bool PluginManager::loadLibraries( const std::string &envvar ) {
    lock_type lock( _mutex ) ;
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
      _logger->log<DEBUG5>() << "<!-- Loading shared library : " << library << " -->" << std::endl ;
      auto inserted = checkDuplicateLibs.insert( libBaseName ).second ;
      if ( not inserted ) {
        _logger->log<ERROR>() << std::endl << "<!-- ERROR loading shared library : " << library << std::endl
            << "    ->    Trying to load DUPLICATE library -->" << std::endl << std::endl ;
        return false ;
      }
      void* libPointer = dlopen( library.c_str() , RTLD_LAZY | RTLD_GLOBAL) ;
      if( nullptr == libPointer ) {
        _logger->log<ERROR>() << std::endl << "<!-- ERROR loading shared library : " << library << std::endl
                    << "    ->    "   << dlerror() << " -->" << std::endl << std::endl ;
        return false ;
      }
      _libraries.push_back( libPointer ) ;
    }
    return true ;
  }

  //--------------------------------------------------------------------------

  void PluginManager::dump() const {
    lock_type lock( _mutex ) ;
    _logger->log<MESSAGE>() << "------------------------------------" << std::endl ;
    _logger->log<MESSAGE>() << " ** Marlin plugin manager dump ** " << std::endl ;
    for ( auto pluginIter : _pluginFactories ) {
      auto typeStr = pluginTypeToString( pluginIter.first ) ;
      if ( pluginIter.second.empty() ) {
        _logger->log<MESSAGE>() << " No " << typeStr << " plugin entry !" << std::endl ;
      }
      else {
        _logger->log<MESSAGE>() << " " << typeStr << " plugins:" << std::endl ;
        for ( auto iter : pluginIter.second ) {
          _logger->log<MESSAGE>() << " - " << iter.first << std::endl ;
        }
      }
    }
    _logger->log<MESSAGE>() << "----------------------------------" << std::endl ;
  }

  //--------------------------------------------------------------------------

  std::string PluginManager::pluginTypeToString( PluginType type ) {
    switch ( type ) {
      case PluginType::Processor: return "Processor" ;
      case PluginType::GeometryPlugin: return "GeometryPlugin" ;
      case PluginType::DataSource: return "DataSource" ;
      default: throw;
    }
  }

  //--------------------------------------------------------------------------

  PluginManager::Logger PluginManager::logger() const {
    return _logger ;
  }

} // namespace marlin
