#include "Processor.h"
#include "ProcessorMgr.h" 

using namespace lcio ;




Processor::Processor(const std::string& typeName) :
  _typeName( typeName ) ,
  _parameters(0) 
{
  
  //register processor in map
  ProcessorMgr::instance()->registerProcessor( this ) ;
}


Processor::Processor(){}

Processor::~Processor() {

  if( _parameters != 0 ){
    delete _parameters ;
  }
  
}


void Processor::setParameters( StringParameters* parameters) {

  if( _parameters != 0 ){
    delete _parameters ;
    _parameters = 0 ;
  }
  _parameters = parameters ;
  
}
