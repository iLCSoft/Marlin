#include "marlin/ProcessorParameter.h"
#include <algorithm>

namespace marlin {

//   const std::string ProcessorParameter::type() {

//     if( typeid( _this ) == typeid( StringVec ) )
//       return "StringVec" ;

//     else
//       return typeid( _parameter ).name() ;

//   }


  void toStream(  std::ostream& s, int i , int ) { s << i ; }
  void toStream(  std::ostream& s, float f , int ) { s << f ; }
  void toStream(  std::ostream& s, double d , int ) { s << d ; }
  void toStream(  std::ostream& s, const std::string& str , int ) { s << str ; }
  void toStream(  std::ostream& s, bool b , int ) { s << ( b ? "true":"false" ) ; }

  template<>
  void setProcessorParameter<int>( ProcessorParameter_t<int>* procParam,  StringParameters* params ) {
    if( params->isParameterSet( procParam->name() ) ) {
      procParam->_parameter = params->getValue<int>( procParam->_name, 0 ) ;
      procParam->_valueSet = true ;
    }
  }
  template<>
  void setProcessorParameter<float>( ProcessorParameter_t<float>* procParam ,  StringParameters* params ) {
    if( params->isParameterSet( procParam->name() ) ) {
      procParam->_parameter = params->getValue<float>( procParam->_name, 0.f ) ;
      procParam->_valueSet = true ;
    }
  }
  template<>
  void setProcessorParameter<double>( ProcessorParameter_t<double>* procParam ,  StringParameters* params ) {
    if( params->isParameterSet( procParam->name() ) ) {
      procParam->_parameter = params->getValue<double>( procParam->_name, 0. ) ;
      procParam->_valueSet = true ;
    }
  }
  template<>
  void setProcessorParameter<std::string>( ProcessorParameter_t<std::string>* procParam ,  StringParameters* params ) {
    if( params->isParameterSet( procParam->name() ) ) {
      procParam->_parameter = params->getValue<std::string>( procParam->_name, "" ) ;
      procParam->_valueSet = true ;
    }
  }
  template<>
  void setProcessorParameter<IntVec>( ProcessorParameter_t<IntVec>* procParam ,  StringParameters* params ) {
    if( params->isParameterSet( procParam->name() ) ) {
      procParam->_parameter.clear() ;
      procParam->_valueSet = true ;
    }
    procParam->_parameter = params->getValues<int>( procParam->_name, IntVec() ) ;
  }
  template<>
  void setProcessorParameter<FloatVec>( ProcessorParameter_t<FloatVec>* procParam ,  StringParameters* params ) {
    if( params->isParameterSet( procParam->name() ) ) {
      procParam->_parameter.clear() ;
      procParam->_valueSet = true ;
    }
    procParam->_parameter = params->getValues<float>( procParam->_name, FloatVec() ) ;
  }
  template<>
  void setProcessorParameter<StringVec>( ProcessorParameter_t<StringVec>* procParam ,  StringParameters* params ) {
    if( params->isParameterSet( procParam->name() ) ) {
      procParam->_parameter.clear() ;
      procParam->_valueSet = true ;
    }
    procParam->_parameter = params->getValues<std::string>( procParam->_name, StringVec() ) ;
  }
  template<>
  void setProcessorParameter<bool>( ProcessorParameter_t<bool>* procParam ,  StringParameters* params )
  {
    if( params->isParameterSet( procParam->name() ) ) {
      std::string param = params->getValue<std::string>( procParam->_name, "" );
      std::transform( param.begin(),param.end(),param.begin(),
		      (int (*)(int)) std::tolower );
      if ( param == "false" || param == "0" )	{
	procParam->_parameter = false;
	procParam->_valueSet = true;
      }
      else if ( param == "true" || param == "1" ) {
	procParam->_parameter = true;
	procParam->_valueSet = true;
      }
      else {
	std::cerr << "Warning: Parameter ["<< procParam->name()
		  << "] is boolean but neither \"true\" nor \"false\"! "
		  << "Ignoring steering parameter" << std::endl;
	procParam->_valueSet = false;
      }
    }
  }


}
