#include "marlin/CCCollection.h"

namespace marlin{

  CCCollection::CCCollection() :
    _value(""),
    _type(""),
    _name(""),
    _srcProc(0){
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

