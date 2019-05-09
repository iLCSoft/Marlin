#include "marlin/Processor.h"

// -- marlin headers
#include "marlin/PluginManager.h"
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

  void Processor::setParameters( std::shared_ptr<StringParameters> processorParameters) {
    _parameters = processorParameters ;
    updateParameters();
    // need to reset the log level name in case it has not been set by the user
    if( not parameterSet("Verbosity") ) {
      _logLevelName = "" ;
    }
    if( _parameters->isParameterSet( "ProcessorName" ) ) {
      setName( _parameters->getValue<std::string>( "ProcessorName" ) ) ;
    }
  }

  //--------------------------------------------------------------------------

  void Processor::printParameters() const {
    printParameters<MESSAGE>() ;
  }

  //--------------------------------------------------------------------------

  void Processor::printDescription() const {
    std::cout << ".begin My"        <<  type()  << std::endl << "ProcessorType "   <<  type() << std::endl ;
    std::cout << "#---" << description() << std::endl ;
    for( auto i = this->pbegin() ; i != this->pend() ; i ++ ) {
      // ProcessorParameter* p = i->second ;
      std::cout << std::endl
		    << "#\t"  << i->second->description() << std::endl
		    << "#\t type: " << " [" <<  i->second->type() << "]"
		    << std::endl ;
      if( i->second->isOptional() ) {
      	std::cout << "#\t example: " << std::endl ;
      	std::cout << "#\t"   << i->second->name()
      		  << "   "  << i->second->defaultValue()
      		  << std::endl
      		  << std::endl ;
      }
      else {
      	std::cout << "#\t default: " << i->second->defaultValue()
      		  << std::endl ;

      	std::cout << "\t"   << i->second->name()
      		  << "   "  << i->second->defaultValue()
      		  << std::endl
      		  << std::endl ;
      }
    }
    std::cout << ".end -------------------------------------------------"
	      << std::endl
	      << std::endl ;
  }

  //--------------------------------------------------------------------------

  void Processor::printDescriptionXML(std::ostream& stream) const {
    if( name().empty() ){
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
    for( auto i = this->pbegin() ; i != this->pend() ; i ++ ) {
      stream << "  <!--" << i->second->description() << "-->" << std::endl ;
      if( i->second->isOptional() ) {
	      stream << "  <!--parameter name=\"" << i->second->name() << "\" "
	      << "type=\"" << i->second->type() ;
	      if ( isInputCollectionName( i->second->name() ) ) {
	        stream << "\" lcioInType=\"" << getLCIOInType( i->second->name() ) ;
        }
	      if ( isOutputCollectionName( i->second->name() ) ) {
	       stream << "\" lcioOutType=\"" << getLCIOOutType( i->second->name() ) ;
        }
      	stream << "\">"
      	       << i->second->defaultValue()
      	       << " </parameter-->"
      	       << std::endl ;
      }
      else {
	      stream << "  <parameter name=\"" << i->second->name() << "\" "
	       << "type=\"" << i->second->type() ;
      	if ( isInputCollectionName( i->second->name() ) ) {
      	  stream << "\" lcioInType=\"" << getLCIOInType( i->second->name() ) ;
        }
      	if ( isOutputCollectionName( i->second->name() ) ) {
      	  stream << "\" lcioOutType=\"" << getLCIOOutType( i->second->name() ) ;
        }
      	stream  << "\">"
      		<< i->second->defaultValue()
      		<< " </parameter>"
      		<< std::endl ;
      }
    }
    stream << "</processor>"
	   << std::endl
	   << std::endl ;
  }

  //--------------------------------------------------------------------------

  void Processor::baseInit( Application *application ) {
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
    for( auto i = this->pbegin() ; i != this->pend() ; i ++ ) {
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

  const Application &Processor::app() const {
    if ( nullptr == _application ) {
      throw Exception( "Processor::app: app pointer not set!" ) ;
    }
    return *_application ;
  }

  //--------------------------------------------------------------------------

  Application &Processor::app() {
    if ( nullptr == _application ) {
      throw Exception( "Processor::app: app pointer not set!" ) ;
    }
    return *_application ;
  }

} // namespace marlin
