#include <marlin/ProcessorFactory.h>

// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/Application.h>
#include <marlin/StringParameters.h>
#include <marlin/PluginManager.h>

namespace marlin {

  std::shared_ptr<Processor> ProcessorFactory::createProcessor(
    const Application *app,
    const std::string &name, 
    std::shared_ptr<StringParameters> parameters) {
    // first look in the single instance map
    auto clone = parameters->getStringVal( "ProcessorClone" ) ;
    if( "false" == clone ) {
      auto iter = _singleProcessors.find( name ) ;
      if( _singleProcessors.end() != iter ) {
        return iter->second ;
      }
    }
    // else create it from plugin manager
    auto type = parameters->getStringVal( "ProcessorType" ) ;
    if( type.empty() ) {
      throw Exception( "ProcessorFactory::createProcessor: Processor type undefined!" ) ;
    }
    auto &pluginMgr = PluginManager::instance() ;
    auto processor = pluginMgr.create<Processor>( PluginType::Processor, type ) ;
    if ( nullptr == processor ) {
      throw Exception( "ProcessorFactory::createProcessor: processor '" + type + "' not registered !" ) ;
    }
    processor->setName( name ) ;
    processor->setParameters( parameters ) ;
    processor->baseInit( app ) ;
    // handle special parameters
    if( "false" == clone ) {
      _singleProcessors.insert( ProcessorMap::value_type( name, processor ) ) ;
    }
    // TODO deal with processor locks there ...
    // auto lock = parameters->getStringVal( "ProcessorLock" ) ;
    // ...
    return processor ;
  }

}
