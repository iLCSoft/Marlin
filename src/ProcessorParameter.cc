#include "marlin/ProcessorParameter.h"

namespace marlin {

  template<>
  void setProcessorParameter<int>( ProcessorParameter_t<int>* procParam,  StringParameters* params ) {
    procParam->_parameter = params->getIntVal( procParam->_name ) ; 
  }
  template<>
  void setProcessorParameter<float>( ProcessorParameter_t<float>* procParam ,  StringParameters* params ) {
    procParam->_parameter = params->getFloatVal( procParam->_name ) ; 
  }
  template<>
  void setProcessorParameter<std::string>( ProcessorParameter_t<std::string>* procParam ,  StringParameters* params ) {
    procParam->_parameter = params->getStringVal( procParam->_name ) ; 
  }
  template<>
  void setProcessorParameter<FloatVec>( ProcessorParameter_t<FloatVec>* procParam ,  StringParameters* params ) {
    params->getFloatVals( procParam->_name,  procParam->_parameter ) ; 
  }
  template<>
  void setProcessorParameter<StringVec>( ProcessorParameter_t<StringVec>* procParam ,  StringParameters* params ) {
    params->getStringVals( procParam->_name,  procParam->_parameter ) ; 
  }
}
