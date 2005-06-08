#include "marlin/Processor.h"
#include "marlin/ProcessorMgr.h" 


using namespace lcio ;


namespace marlin{


Processor::Processor(const std::string& typeName) :
  _description(" description not set by author ") ,
  _typeName( typeName ) ,
  _parameters(0) ,
  _isFirstEvent( true )
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


  void Processor::printParameters(){
    
    typedef ProcParamMap::iterator PMI ;
    
    std::cout << std::endl  
     	      << "---- " << name()  <<" -  parameters: " << std::endl ;
    
    
    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {
      
      if( ! i->second->isOptional() || i->second->valueSet() ){
	std::cout << "\t"   << i->second->name()   
		  << ":  "  << i->second->value() 
		  << std::endl ;
      }
    }

    std::cout << "-------------------------------------------------" 
	      << std::endl ;
  }


  void Processor::printDescription() {
    
    std::cout << ".begin My"        <<  type()  << std::endl 
	      << "ProcessorType "   <<  type() << std::endl ;
    
    std::cout << "#---" << description() << std::endl ;
    
    typedef ProcParamMap::iterator PMI ;
    
    //     std::cout << std::endl  
    // 	      << "# processor parameters:" << std::endl ;
    
    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {
      
      ProcessorParameter* p = i->second ;

      std::cout << std::endl 
		<< "#\t"  << p->description() << std::endl 
		<< "#\t type: " << " [" <<  p->type() << "]" 
		<< std::endl ;

      if( p->isOptional() ) {
	
	std::cout << "#\t example: " << std::endl ;
	
	std::cout << "#\t"   << p->name()   
		  << "   "  << p->defaultValue() 
		  << std::endl 
		  << std::endl ;
	
      }else{

	std::cout << "#\t default: " << p->defaultValue() 
		  << std::endl ;
	
	std::cout << "\t"   << p->name()   
		  << "   "  << p->defaultValue() 
		  << std::endl 
		  << std::endl ;
      }
      
    }

    std::cout << ".end -------------------------------------------------" 
	      << std::endl 
	      << std::endl ;


  }

  void Processor::printDescriptionXML() {
    
    std::cout << " <processor name=\"My" <<  type()  << "\"" 
	      << " type=\"" <<  type() << "\">" 
	      << std::endl ;
    
    std::cout << " <!--" << description() << "-->" << std::endl ;
    
    typedef ProcParamMap::iterator PMI ;
    
    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {
      
      ProcessorParameter* p = i->second ;

      std::cout << "  <!--" << p->description() << "-->" << std::endl ;

      if( p->isOptional() ) {
	std::cout << "  <!--parameter name=\"" << p->name() << "\" " 
		  << "type=\"" << p->type() << "\">"
		  << p->defaultValue() 
		  << " </parameter-->"
		  << std::endl ;
      } else {
	std::cout << "  <parameter name=\"" << p->name() << "\" " 
		  << "type=\"" << p->type() << "\">"
		  << p->defaultValue() 
		  << " </parameter>"
		  << std::endl ;
      }
    }
    
    std::cout << "</processor>" 
	      << std::endl 
	      << std::endl ;
    
  }

  bool Processor::parameterSet( const std::string& name ) {

    ProcParamMap::iterator it = _map.find(name) ;

    if( it != _map.end() )
      return it->second->valueSet() ;    
    else
      return false ;
  }
  
  void Processor::baseInit() {

    typedef ProcParamMap::iterator PMI ;

    for( PMI i = _map.begin() ; i != _map.end() ; i ++ ) {

      i->second->setValue( _parameters ) ;
    }

    init() ;

  }


  void Processor::setReturnValue( bool val) {
    ProcessorMgr::instance()->setProcessorReturnValue(  this , val ) ;
  }


} // namespace marlin

