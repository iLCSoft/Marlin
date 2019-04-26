#include "marlin/Processor.h"

// -- marlin headers
#include "marlin/PluginManager.h"
#include "marlin/Scheduler.h"
#include "marlin/Application.h"

namespace marlin {

  Processor::Processor( const std::string& typeName ) :
    _description(" description not set by author ") ,
    _typeName( typeName ) ,
    _parameters(0) ,
    _logLevelName("") {
    //register processor
    PluginManager::instance().registerPlugin( PluginType::Processor, type(), [this](){
      return std::shared_ptr<void>( this->newProcessor() ) ;
    }, true ) ;
    // verbosity default parameter
    registerOptionalParameter( "Verbosity" ,
			       "verbosity level of this processor (\"DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT\")"  ,
			       _logLevelName ,
			       std::string("") ) ;
    std::stringstream ss ; ss << _typeName <<  "_" << (void*)this ;
    _logger = Logging::createLogger( ss.str() ) ;
    _logger->setLevel( "MESSAGE" );
  }

  //--------------------------------------------------------------------------

  Processor::~Processor() {
    for( auto i = _map.begin() ; i != _map.end() ; i ++ ) {
      delete i->second ;
    }
  }

  //--------------------------------------------------------------------------

  void Processor::setParameters( std::shared_ptr<StringParameters> processorParameters) {
    _parameters = processorParameters ;
    updateParameters();
    // need to reset the log level name in case it has not been set by the user
    if( not parameterSet("Verbosity") ) {
      _logLevelName = "" ;
    }
  }

  //--------------------------------------------------------------------------

  void Processor::printParameters() {
    printParameters<MESSAGE>() ;
  }

  //--------------------------------------------------------------------------

  void Processor::printDescription() {
    std::cout << ".begin My"        <<  type()  << std::endl << "ProcessorType "   <<  type() << std::endl ;
    std::cout << "#---" << description() << std::endl ;
    for( auto i = _map.begin() ; i != _map.end() ; i ++ ) {
      ProcessorParameter* p = i->second ;
      std::cout << std::endl
		    << "#\t"  << p->description() << std::endl
		    << "#\t type: " << " [" <<  p->type() << "]"
		    << std::endl ;
      if( p->isOptional() ) {
      	std::cout << "#\t example: " << std::endl ;
      	std::cout << "#\t"   << p->name()
      		  << "   "  << p->defaultValue()
      		  << std::endl
      		  << std::endl ;
      }
      else {
      	std::cout << "#\t default: " << p->defaultValue()
      		  << std::endl ;

      	std::cout << "\t"   << p->name()
      		  << "   "  << p->defaultValue()
      		  << std::endl
      		  << std::endl ;
      }
    }
    std::cout << ".end -------------------------------------------------"
	      << std::endl
	      << std::endl ;
  }

  //--------------------------------------------------------------------------

  void Processor::printDescriptionXML(std::ostream& stream) {
    if(&stream == &std::cout){
      stream << " <processor name=\"My" <<  type()  << "\""
	     << " type=\"" <<  type() << "\">"
	     << std::endl ;
    }
    else {
      stream << " <processor name=\"" <<  name()  << "\""
	     << " type=\"" <<  type() << "\">"
	     << std::endl ;
    }
    stream << " <!--" << description() << "-->" << std::endl ;
    for( auto i = _map.begin() ; i != _map.end() ; i ++ ) {
      ProcessorParameter* p = i->second ;
      stream << "  <!--" << p->description() << "-->" << std::endl ;
      if( p->isOptional() ) {
	      stream << "  <!--parameter name=\"" << p->name() << "\" "
	      << "type=\"" << p->type() ;
	      if ( isInputCollectionName( p->name() ) ) {
	        stream << "\" lcioInType=\"" << _inTypeMap[ p->name() ]  ;
        }
	      if ( isOutputCollectionName( p->name() ) ) {
	       stream << "\" lcioOutType=\"" << _outTypeMap[ p->name() ]  ;
        }
      	stream << "\">"
      	       << p->defaultValue()
      	       << " </parameter-->"
      	       << std::endl ;
      }
      else {
	      stream << "  <parameter name=\"" << p->name() << "\" "
	       << "type=\"" << p->type() ;
      	if ( isInputCollectionName( p->name() ) ) {
      	  stream << "\" lcioInType=\"" << _inTypeMap[ p->name() ]  ;
        }
      	if ( isOutputCollectionName( p->name() ) ) {
      	  stream << "\" lcioOutType=\"" << _outTypeMap[ p->name() ]  ;
        }
      	stream  << "\">"
      		<< p->defaultValue()
      		<< " </parameter>"
      		<< std::endl ;
      }
    }
    stream << "</processor>"
	   << std::endl
	   << std::endl ;
  }

  //--------------------------------------------------------------------------

  bool Processor::parameterSet( const std::string& parameterName ) {
    auto it = _map.find(parameterName) ;
    if( it != _map.end() ) {
      return it->second->valueSet() ;
    }
    else {
      return false ;
    }
  }

  //--------------------------------------------------------------------------

  void Processor::baseInit( const Application *application ) {
    _application = application ;
    _logger = app().createLogger( name() ) ;
    log<DEBUG2>() << "Creating logger for processor " << name() << std::endl ;
    if( parameterSet("Verbosity") ) {
      _logger->setLevel( _logLevelName ) ;
    }
    log<DEBUG2>() << "Processor " << name() << ": init ..." << std::endl ;
    init() ;
  }

  //--------------------------------------------------------------------------

  void Processor::updateParameters() {
    for( auto i = _map.begin() ; i != _map.end() ; i ++ ) {
      i->second->setValue( _parameters.get() ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  std::pair<bool,bool> Processor::getForcedRuntimeOption( RuntimeOption option ) const {
    std::pair<bool,bool> result ;
    result.first = false ;
    auto iter = _forcedRuntimeOptions.find( option ) ;
    if( _forcedRuntimeOptions.end() != iter ) {
      result.first = true ;
      result.second = iter->second ;
    }
    return result ;
  }
  
  //--------------------------------------------------------------------------
  
  void Processor::forceRuntimeOption( RuntimeOption option, bool value ) {
    _forcedRuntimeOptions[option] = value ;
  }

  //--------------------------------------------------------------------------

  void Processor::setReturnValue( bool val ) {
    if ( nullptr == _scheduler ) {
      throw Exception( "Processor::setReturnValue: Processor not initialized !" ) ;
    }
    _scheduler->runtimeConditions()->setValue( name(), val ) ;
  }

  //--------------------------------------------------------------------------

  void Processor::setLCIOInType(const std::string& collectionName,  const std::string& lcioInType) {
    _inTypeMap[ collectionName ] = lcioInType ;
  }

  //--------------------------------------------------------------------------

  std::string Processor::getLCIOInType( const std::string& colName ) {
    if( isInputCollectionName( colName )  )
      return _inTypeMap[ colName ] ;
    else
      return "" ;
  }

  //--------------------------------------------------------------------------

  std::string Processor::getLCIOOutType( const std::string& colName ) {
    if( isOutputCollectionName( colName )  )
      return _outTypeMap[ colName ] ;
    else
      return "" ;
  }

  //--------------------------------------------------------------------------

  bool Processor::isInputCollectionName( const std::string& pName  ) {
    return _inTypeMap.find( pName  ) != _inTypeMap.end() ;
  }

  //--------------------------------------------------------------------------

  void Processor::setLCIOOutType(const std::string& collectionName,  const std::string& lcioOutType) {
    _outTypeMap[ collectionName ] = lcioOutType ;
  }

  //--------------------------------------------------------------------------

  bool Processor::isOutputCollectionName( const std::string& pName  ) {
    return _outTypeMap.find( pName ) != _outTypeMap.end() ;
  }

  //--------------------------------------------------------------------------

  void Processor::setReturnValue( const std::string& keyName, bool val ) {
    if ( nullptr == _scheduler ) {
      throw Exception( "Processor::setReturnValue: Processor not initialized !" ) ;
    }
    std::string valName = name() + "." + keyName ;
    _scheduler->runtimeConditions()->setValue( valName, val ) ;
  }
  
  //--------------------------------------------------------------------------
  
  const Application &Processor::app() const {
    if ( nullptr == _application ) {
      throw Exception( "Processor::app: app pointer not set!" ) ;
    }
    return *_application ;
  }

} // namespace marlin
