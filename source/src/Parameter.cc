#include "marlin/Parameter.h"

// -- marlin headers
#include "marlin/LCIOSTLTypes.h"

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

  void Parametrized::checkForExistingParameter( const std::string& pname ) {
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
  
  EVENT::StringVec Parametrized::parameterNames() const {
    EVENT::StringVec names {} ;
    for( auto iter : _parameters ) {
      names.push_back( iter.first ) ;
    }
    return names ;
  }
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <>
  void ParameterT<int>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      _value = params->getIntVal( name() ) ; 
      _valueSet = true ;
    }
  }
  
  //--------------------------------------------------------------------------

  template <>
  void ParameterT<float>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      _value = params->getFloatVal( name() ) ; 
      _valueSet = true ;
    }
  }
  
  //--------------------------------------------------------------------------

  template <>
  void ParameterT<double>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      _value = params->getFloatVal( name() ) ; 
      _valueSet = true ;
    }
  }
  
  //--------------------------------------------------------------------------

  template <>
  void ParameterT<std::string>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      _value = params->getStringVal( name() ) ; 
      _valueSet = true ;
    }
  }
  
  //--------------------------------------------------------------------------

  template <>
  void ParameterT<EVENT::IntVec>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      _value.clear() ;
      _valueSet = true ;
    }
    params->getIntVals( name(), _value ) ; 
  }
  
  //--------------------------------------------------------------------------

  template <>
  void ParameterT<EVENT::FloatVec>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      _value.clear() ;
      _valueSet = true ;
    }
    params->getFloatVals( name(), _value ) ; 
  }
  
  //--------------------------------------------------------------------------

  template <>
  void ParameterT<EVENT::StringVec>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      _value.clear() ;
      _valueSet = true ;
    }
    params->getStringVals( name(), _value ) ; 
  }
  
  //--------------------------------------------------------------------------

  template <>
  void ParameterT<bool>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      std::string param = params->getStringVal( name() ) ;
      std::transform( param.begin(), param.end(), param.begin(), (int (*)(int)) std::tolower ) ;
      if ( param == "false" || param == "0" )	{
      	_value = false ;
      	_valueSet = true ;
      }
      else if ( param == "true" || param == "1" ) {
      	_value = true ;
      	_valueSet = true ;
      }
      else {
      	streamlog_out(WARNING) << "Warning: Parameter ["<< name()
      		  << "] is boolean but neither \"true\" nor \"false\"! "
      		  << "Ignoring steering parameter" << std::endl ;
      	_valueSet = false ;
      }      
    }
  }  
}
