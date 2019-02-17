#include "marlin/concurrency/Parameters.h"

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

  } // namespace concurrency

} // namespace marlin
