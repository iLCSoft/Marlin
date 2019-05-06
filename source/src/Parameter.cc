#include "marlin/Parameter.h"

// -- marlin headers
#include "marlin/Logging.h"

// -- std headers
#include <algorithm>

namespace marlin {

  const std::string &Parameter::name() const {
    return _name ;
  }

  //--------------------------------------------------------------------------

  const std::string &Parameter::description() const {
    return _description ;
  }

  //--------------------------------------------------------------------------

  int Parameter::setSize() const {
    return _setSize ;
  }

  //--------------------------------------------------------------------------

  bool Parameter::isOptional() const {
    return _optional ;
  }

  //--------------------------------------------------------------------------

  bool Parameter::valueSet() const {
    return _valueSet ;
  }

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  bool Parametrized::parameterSet( const std::string& pname ) {
    auto iter = _parameters.find( pname ) ;
    return ( iter != _parameters.end() ? iter->second->valueSet() : false ) ;
  }

  //--------------------------------------------------------------------------

  void Parametrized::checkForExistingParameter( const std::string& pname ) const {
    auto iter = _parameters.find( pname ) ;
    if (iter != _parameters.end() ) {
      throw Exception( "Parameter " + pname + " already defined") ;
    }
  }

  //--------------------------------------------------------------------------

  void Parametrized::clearParameters() {
    _parameters.clear() ;
  }

  //--------------------------------------------------------------------------

  void Parametrized::setParameters( std::shared_ptr<StringParameters> parameters ) {
    for( auto iter : _parameters ) {
      iter.second->setValue( parameters.get() ) ;
    }
  }

  //--------------------------------------------------------------------------

  std::vector<std::string> Parametrized::parameterNames() const {
    std::vector<std::string> names {} ;
    for( auto iter : _parameters ) {
      names.push_back( iter.first ) ;
    }
    return names ;
  }

}
