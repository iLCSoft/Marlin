#include "marlin/CCCollection.h"

namespace marlin{

  CCCollection::CCCollection() :
    _value(""),
    _type(""),
    _name(""),
    _srcProc(0){
  }
 
  CCCollection::CCCollection(CCCollection const &c) : 
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

