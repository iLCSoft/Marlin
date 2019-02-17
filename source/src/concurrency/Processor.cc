#include "marlin/concurrency/Processor.h"

namespace marlin {

  namespace concurrency {

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
      std::cout << "---- " << name() << " -  parameters:" << std::endl ;
      _parameters.print() ;
      std::cout << "-------------------------------------------------" << std::endl ;
    }

    //--------------------------------------------------------------------------

    void Processor::registerProcessorParameter( const std::string &parameterName,
      const std::string &parameterDescription ) {
      if ( _parameters.exists( parameterName ) ) {
        throw Exception( "Parameter '" + parameterName + "' registered twice!" );
      }
      Parameter &param = _parameters.create( parameterName ) ;
      param.setDescription( parameterDescription );
    }

  } // namespace concurrency

} // namespace marlin
