#include "marlin/Processor.h"
#include "marlin/ProcessorMgr.h" 


using namespace lcio ;


namespace marlin{


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



//   template<class T>
//   export void Processor::registerProcessorParameter(const std::string& name, 
// 					     const std::string&description,
// 					     T& parameter ) {
    
    
//     _map[ name ] = new ProcessorParameter_t<T>( name , description, parameter ) ;


//   }

  void Processor::dump() {

    std::cout << ".begin My"        <<  type()  << std::endl 
	      << "ProcessorType "   <<  type() << std::endl ;
    
    typedef ProcParamMap::iterator PMI ;
    
//     std::cout << std::endl  
// 	      << "# processor parameters:" << std::endl ;
    
    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {
      

      std::cout << std::endl 
		<< "#\t"  << i->second->description() << std::endl ;

      std::cout << "#\t default: " << i->second->defaultValue() 
		<< " [" <<  i->second->type() << "]" << std::endl ;

      std::cout << "\t"   << i->second->name()   
		<< "   "  << i->second->defaultValue() 
		<< std::endl 
		<< std::endl ;
      

    }

    std::cout << ".end -------------------------------------------------" 
	      << std::endl 
	      << std::endl ;


//     std::cout << "\tProcessor : " << type() << std::endl ;
    
//     typedef ProcParamMap::iterator PMI ;

//     std::cout << "\t\tprocessor parameters:" << std::endl ;

//     for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {

//       std::cout << "\t\t  name: " << i->second->name()   
// 		<< "  description: " << i->second->description() 
// 		<< "  type: " << i->second->type() 
// 		<< "  default: " << i->second->defaultValue() 
// 		<< std::endl ;
//     }

  }
  
  void Processor::baseInit() {

    typedef ProcParamMap::iterator PMI ;


    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {

      i->second->setValue( _parameters ) ;

    }

    init() ;

  }



} // namespace marlin

