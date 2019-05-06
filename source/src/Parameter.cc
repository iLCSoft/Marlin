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

  void Parametrized::registerInputCollection(const std::string& collectionType,
        const std::string& parameterName,
        const std::string& parameterDescription,
        std::string& parameter,
        const std::string& defaultVal,
        int setSize ) {
    setLCIOInType( parameterName , collectionType ) ;
    registerParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ;      
  }
        
  //--------------------------------------------------------------------------
        
  void Parametrized::registerInputCollections(const std::string& collectionType,
        const std::string& parameterName,
        const std::string& parameterDescription,
        std::vector<std::string>& parameter,
        const std::vector<std::string>& defaultVal,
        int setSize ) {
    setLCIOInType( parameterName , collectionType ) ;
    registerParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ;
  }

  //--------------------------------------------------------------------------
        
  void Parametrized::registerOutputCollection(const std::string& collectionType,
        const std::string& parameterName,
        const std::string& parameterDescription,
        std::string& parameter,
        const std::string& defaultVal,
        int setSize ) {
    setLCIOOutType( parameterName , collectionType ) ;
    registerParameter( parameterName, parameterDescription, parameter, defaultVal, setSize ) ;
  }

  //--------------------------------------------------------------------------

  bool Parametrized::parameterSet( const std::string& pname ) {
    auto iter = _parameters.find( pname ) ;
    return ( iter != _parameters.end() ? iter->second->valueSet() : false ) ;
  }

  //--------------------------------------------------------------------------

  void Parametrized::checkForExistingParameter( const std::string& pname ) const {
    auto iter = _parameters.find( pname ) ;
    if (iter != _parameters.end() ) {
      throw Exception( "Parameter " + pname + " already defined" ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void Parametrized::setLCIOInType(const std::string& collectionName, const std::string& lcioInType) {
    _inTypeMap[ collectionName ] = lcioInType ;
  }
  
  //--------------------------------------------------------------------------

  void Parametrized::setLCIOOutType(const std::string& collectionName,  const std::string& lcioOutType) {
    _outTypeMap[ collectionName ] = lcioOutType ;
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
  
  //--------------------------------------------------------------------------
  
  std::string Parametrized::getLCIOInType( const std::string& colName ) const {
    auto iter = _inTypeMap.find( colName ) ;
    return (iter != _inTypeMap.end()) ? iter->second : "" ;
  }
  
  //--------------------------------------------------------------------------

  std::string Parametrized::getLCIOOutType( const std::string& colName ) const {
    auto iter = _outTypeMap.find( colName ) ;
    return (iter != _outTypeMap.end()) ? iter->second : "" ;
  }
  
  //--------------------------------------------------------------------------
  
  bool Parametrized::isInputCollectionName( const std::string& parameterName ) const {
    return (_inTypeMap.find( parameterName ) != _inTypeMap.end() ) ;
  }
  
  //--------------------------------------------------------------------------

  bool Parametrized::isOutputCollectionName( const std::string& parameterName ) const {
    return (_outTypeMap.find( parameterName ) != _outTypeMap.end() ) ;
  }
  
  //--------------------------------------------------------------------------
  
  Parametrized::iterator Parametrized::pbegin() {
    return _parameters.begin() ;
  }
  
  //--------------------------------------------------------------------------

  Parametrized::iterator Parametrized::pend() {
    return _parameters.end() ;
  }
  
  //--------------------------------------------------------------------------

  Parametrized::const_iterator Parametrized::pbegin() const {
    return _parameters.begin() ;
  }
  
  //--------------------------------------------------------------------------

  Parametrized::const_iterator Parametrized::pend() const {
    return _parameters.end() ;
  }

}
