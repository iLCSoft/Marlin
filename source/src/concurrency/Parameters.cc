#include "marlin/concurrency/Parameters.h"

// -- std headers
#include <sstream>

namespace marlin {

  namespace concurrency {

    Parameter::Parameter() :
      _initialized(false),
      _type(typeid(nullptr)) {
      /* nop */
    }

    //--------------------------------------------------------------------------

    Parameter::Parameter( const Parameter &parameter ) :
      _type(typeid(nullptr)) {
      *this = parameter ;
    }

    //--------------------------------------------------------------------------

    Parameter& Parameter::operator=(const Parameter &rhs) {
      _initialized = rhs._initialized ;
      _type = rhs._type ;
      _parameter = rhs._parameter ;
      return *this ;
    }

    //--------------------------------------------------------------------------

    const std::type_index &Parameter::type() const {
      return _type ;
    }

    //--------------------------------------------------------------------------

    bool Parameter::isInitialized() const {
      return _initialized ;
    }

    //--------------------------------------------------------------------------

    void Parameter::reset() {
      _initialized = false ;
      _parameter.clear() ;
      _type = std::type_index( typeid(nullptr) ) ;
    }

    //--------------------------------------------------------------------------

    void Parameter::setDescription( const std::string &desc ) {
      _description = desc;
    }

    //--------------------------------------------------------------------------

    const std::string &Parameter::description() const {
      return _description;
    }

    //--------------------------------------------------------------------------

    std::string Parameter::asString() const {
      std::ostringstream oss ;
      print(oss) ;
      return oss.str() ;
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    bool Parameters::isList( const std::string &name ) const {
      auto iter = _parameters.find( name ) ;
      if( _parameters.end() == iter ) {
        throw Exception( "Parameters::isList: parameter '" + name + "' not found!" ) ;
      }
      return iter->second.isList() ;
    }

    //--------------------------------------------------------------------------

    std::string Parameters::description( const std::string &name ) const {
      auto iter = _parameters.find( name ) ;
      if( _parameters.end() == iter ) {
        throw Exception( "Parameters::description: parameter '" + name + "' not found!" ) ;
      }
      return iter->second.description() ;
    }

    //--------------------------------------------------------------------------

    std::string Parameters::asString( const std::string &name ) const {
      auto iter = _parameters.find( name ) ;
      if( _parameters.end() == iter ) {
        throw Exception( "Parameters::asString: parameter '" + name + "' not found!" ) ;
      }
      return iter->second.asString() ;
    }

    //--------------------------------------------------------------------------

    std::vector<std::string> Parameters::uninitializedParameterNames() const  {
      std::vector<std::string> params {} ;
      for ( auto iter : _parameters ) {
        if ( not iter.second.isInitialized() ) {
          params.push_back( iter.first ) ;
        }
      }
      return params ;
    }

  } // namespace concurrency

} // namespace marlin
