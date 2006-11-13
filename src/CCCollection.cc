#include "marlin/CCCollection.h"

#include <iostream>

namespace marlin{

  CCCollection::CCCollection( const std::string& value, const std::string& type, const std::string& name, CCProcessor* srcProc ) :
    _value(value),
    _type(type),
    _name(name),
    _srcProc(srcProc){
  }
 
  CCCollection::~CCCollection(){
      _srcProc=NULL;
  }
  
  CCCollection::CCCollection(const CCCollection &c) : 
    _value(c._value),
    _type(c._type),
    _name(c._name),
    _srcProc(c._srcProc){
  }
  
  void CCCollection::setValue( const std::string& value ){
    _value = value;
  }
                                                                                                                                                             
  void CCCollection::setType( const std::string& type ){
    _type = type;
  }

  void CCCollection::setName( const std::string& name ){
    _name = name;
  }
                                                                                                                                                            
  void CCCollection::setSrcProc( CCProcessor* srcProc ){
    _srcProc = srcProc;
  }

} // namespace 

