#ifndef ProcessorParameter_h
#define ProcessorParameter_h 1

#include "lcio.h"

#include <iostream>
#include <string>
#include <sstream>
#include "LCIOSTLTypes.h"
#include "StringParameters.h"
#include <typeinfo>

// typedef std::map< std::string ,  std::vector< std::string > >  ParametersMap ;


using namespace lcio ;

namespace marlin{
  
  /** Class that holds a steering variable for a marlin processor.
   */
  
  class ProcessorParameter {
    
    friend std::ostream& operator<< (  std::ostream& , ProcessorParameter& ) ;
    
  public:
    
    ProcessorParameter() {} 
    
    virtual ~ProcessorParameter() {} 
    
    virtual const std::string&  name() { return _name ; } 
    virtual const std::string&  description()  { return _description ; } 

    
    virtual const std::string  type()=0 ;
    virtual const std::string  value()=0 ;
    virtual const std::string  defaultValue()=0 ;
    
    
    
    virtual void setValue(  StringParameters* params )=0 ;
    
  protected:
    
    std::string _description ;
    std::string _name ;
  };
  
  
  std::ostream& operator<< (  std::ostream& s, ProcessorParameter& p ) ;
  
  
  
  template< class T>
  class ProcessorParameter_t ;
  
  template < class T1>
  void setProcessorParameter( ProcessorParameter_t<T1>* procParam ,  StringParameters* params );
  
  
  
  template< class T>
  class ProcessorParameter_t : public ProcessorParameter {
    
    friend void setProcessorParameter<>(ProcessorParameter_t<T>* ,  StringParameters* ) ;
    
  public:
    
    ProcessorParameter_t( const std::string& name,
			  const std::string& description, 
			  T& parameter ,    
 			  const T& defaultValue ) :     
      _parameter( parameter ),
      _defaultValue( defaultValue ) 
    {
      _name = name ;
      _parameter = defaultValue ;
      _description = description ;
    }
    
    virtual ~ProcessorParameter_t() {}
    
    
    
    //    virtual const std::string  name() { return _name ; } 

    virtual const std::string  type() { return typeid( _parameter ).name() ; } 
    
    virtual const std::string  defaultValue() {

     std::stringstream def ;

     def << _defaultValue << std::ends ;

     return def.str() ;
    }

    virtual const std::string  value() {

     std::stringstream def ;

     def << _parameter << std::ends ;

     return def.str() ;
    }

    
    void setValue(  StringParameters* params ) {
      
      setProcessorParameter< T >( this , params ) ;
      
    }
    
    
    
  protected:
    T& _parameter ;
    T _defaultValue ;
    
  };
  
} // end namespace marlin 
#endif


//#include "ProcessorParameter.icc"
