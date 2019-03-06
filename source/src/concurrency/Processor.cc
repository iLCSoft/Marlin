#include "marlin/concurrency/Processor.h"
#include "marlin/concurrency/XMLHelper.h"

namespace marlin {

  namespace concurrency {

    Processor::Processor( const std::string &t ) :
      _type(t) {
      // TODO deal with logging in MT environment ...
      // registerOptionalParameter( "Verbosity", "The verbosity level for this processor", "MESSAGE" ) ;
    }

    //--------------------------------------------------------------------------

    const std::string &Processor::type() const {
      return _type ;
    }

    //--------------------------------------------------------------------------

    const std::string &Processor::name() const {
      return _name ;
    }

    //--------------------------------------------------------------------------

    const std::string &Processor::description() const {
      return _description ;
    }

    //--------------------------------------------------------------------------

    const Application &Processor::app() const {
      if ( nullptr == _application ) {
        throw Exception( "Application not initialized" );
      }
      return *_application ;
    }

    //--------------------------------------------------------------------------

    Application &Processor::app() {
      if ( nullptr == _application ) {
        throw Exception( "Application not initialized" );
      }
      return *_application ;
    }

    //--------------------------------------------------------------------------

    const Parameters &Processor::parameters() const {
      return _parameters ;
    }

    //--------------------------------------------------------------------------

    void Processor::setName( const std::string &n ) {
      _name = n;
    }

    //--------------------------------------------------------------------------

    void Processor::setApp( Application *application ) {
      _application = application ;
    }

    //--------------------------------------------------------------------------

    void Processor::printParameters () const {
      std::cout << "---- " << name() ;
      if ( _isCritical ) {
        std::cout << " [critical]" ;
      }
      std::cout << " -  parameters:" << std::endl ;
      _parameters.print() ;
      std::cout << "-------------------------------------------------" << std::endl ;
    }
    
    //--------------------------------------------------------------------------
    
    bool Processor::isCritical() const {
      return _isCritical ;
    }

    //--------------------------------------------------------------------------

    void Processor::registerParameter( const std::string &parameterName,
      const std::string &parameterDescription ) {
      if ( _parameters.exists( parameterName ) ) {
        throw Exception( "Parameter '" + parameterName + "' registered twice!" );
      }
      Parameter &param = _parameters.create( parameterName ) ;
      param.setDescription( parameterDescription );
    }

    //--------------------------------------------------------------------------

    void Processor::parseParameters( const TiXmlElement *const element ) {
      XMLHelper::iterateParameters( element, [this]( const TiXmlElement *const child, const std::string &name, const std::string &value ){
        const bool exists = this->_parameters.exists( name ) ;
        if ( not exists ) {
          throw Exception(
            "Processor::parseParameters: processor parameter '" + name + "' not registered by processor.\n" +
            "Check the consistency between your processor parameters defintion and the steering file." ) ;
        }
        // Check whether the parameter value is a list
        bool isList = XMLHelper::readAttribute<bool>( child, "list", false ) ;
        if ( isList ) {
          auto tokens = StringUtil::split<std::string>( value ) ;
          this->_parameters.set( name, tokens ) ;
        }
        else {
          this->_parameters.set( name, value ) ;
        }
        return true ;
      });
      auto uninit = _parameters.uninitializedParameterNames() ;
      if ( not uninit.empty() ) {
        std::stringstream ss ;
        ss << "Processor::parseParameters: The following processor parameters do " <<
        "not provide default values and have not been found in the steering file:" << std::endl;
        for ( auto param : uninit ) {
          ss << "  - " << param << std::endl ;
        }
        throw Exception( ss.str() ) ;
      }
    }

    //--------------------------------------------------------------------------

    void Processor::populateXMLParameters( TiXmlElement *element ) const {
      auto names = _parameters.names() ;
      for ( auto pname : names ) {
        // Add the parameter description as comment just before the parameter element itself
        TiXmlComment *descElement = new TiXmlComment( _parameters.description( pname ).c_str() ) ;
        element->LinkEndChild( descElement ) ;
        // Add the parameter element and populate with metadata
        TiXmlElement *parElement = new TiXmlElement( "parameter" ) ;
        element->LinkEndChild( parElement ) ;
        if( _parameters.isList( pname ) ) {
          parElement->SetAttribute( "list", "true" );
        }
        parElement->SetAttribute( "name", pname ) ;
        if ( _parameters.isInitialized( pname ) ) {
          parElement->SetAttribute( "value", _parameters.asString( pname ) ) ;
        }
      }
    }

  } // namespace concurrency

} // namespace marlin
