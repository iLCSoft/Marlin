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
  
  /** Class that holds a steering variable for a marlin processor - automatically created by
   *  Processor::registerProcessorParameter() 
   *
   *  @author F. Gaede, DESY
   *  @version $Id: ProcessorParameter.h,v 1.9 2006-08-03 15:16:34 gaede Exp $ 
   */
  
  class ProcessorParameter {
    
    friend std::ostream& operator<< (  std::ostream& , ProcessorParameter& ) ;
    
  public:
    
    ProcessorParameter() {} 
    
    virtual ~ProcessorParameter() {} 
    
    virtual const std::string&  name() { return _name ; } 
    virtual const std::string&  description()  { return _description ; } 
    virtual int setSize() { return _setSize ; } ;
    virtual bool isOptional() { return _optional ; }
    virtual bool valueSet() { return _valueSet ; }
    
    virtual const std::string  type()=0 ;
    virtual const std::string  value()=0 ;
    virtual const std::string  defaultValue()=0 ;
    
    
    
    virtual void setValue(  StringParameters* params )=0 ;
    
  protected:
    
    std::string _description ;
    std::string _name ;
    int _setSize ;
    bool _optional ;
    bool _valueSet ;
  };
  
  
  std::ostream& operator<< (  std::ostream& s, ProcessorParameter& p ) ;
  
  
  /**Helper function for printing parameter vectors.
   */
//   template< class T>
//   std::ostream& operator<< (  std::ostream& s, const std::vector<T>& v ) {
    
//     typename std::vector<T>::const_iterator it ;
    
//     for( it = v.begin() ; it != v.end() ; it++) {
//       s <<  (*it) << " " ;
//     }
//     return s ;
//   }

  void toStream(  std::ostream& s, int i , int N) ; 
  void toStream(  std::ostream& s, float f , int N) ;
  void toStream(  std::ostream& s, double d , int N) ;
  void toStream(  std::ostream& s, const std::string& str , int N) ; 
  void toStream(  std::ostream& s, bool b , int N) ;
  
  template< class T>
  std::ostream& toStream(  std::ostream& s, const std::vector<T>& v , int N) {
    
    typename std::vector<T>::const_iterator it ;
    
    unsigned count = 0 ;
    for( it = v.begin() ; it != v.end() ; it++) {

      if( count && N  && ! (count % N)  )   // start a new line after N parameters
	s << "\n\t\t"  ; 

      s <<  (*it) << " " ;
      count ++ ;


    }
    return s ;
  }


  template< class T>
  class ProcessorParameter_t ;
  
  template < class T1>
  void setProcessorParameter( ProcessorParameter_t<T1>* procParam ,  StringParameters* params );
  
  
  
  /** Templated implementation of ProcessorParameter - automatically created by
      Processor::registerProcessorParameter() */
  template< class T>
  class ProcessorParameter_t : public ProcessorParameter {
    
    friend void setProcessorParameter<>(ProcessorParameter_t<T>* ,  StringParameters* ) ;
    
  public:
    
    ProcessorParameter_t( const std::string& name,
			  const std::string& description, 
			  T& parameter ,    
 			  const T& defaultValue,
			  bool optional,
			  int setSize=0) :     

      _parameter( parameter ),
      _defaultValue( defaultValue ) 
    {
      _name = name ;
      _parameter = defaultValue ;
      _description = description ;
      _optional = optional ;
      _valueSet = false ;
      _setSize = setSize ;
    }

    virtual ~ProcessorParameter_t() {}
    
    
    
    //    virtual const std::string  name() { return _name ; } 

    virtual const std::string  type() {  

      // return typeid( _parameter ).name() ; } 

      // make this human readable
      if     ( typeid( _parameter ) == typeid( IntVec    )) return "IntVec" ;
      else if( typeid( _parameter ) == typeid( FloatVec  )) return "FloatVec" ;
      else if( typeid( _parameter ) == typeid( StringVec )) return "StringVec" ;
      else if( typeid( _parameter ) == typeid( int   )) return "int" ;
      else if( typeid( _parameter ) == typeid( float )) return "float" ;
      else if( typeid( _parameter ) == typeid( double )) return "double" ;
      else if( typeid( _parameter ) == typeid(std::string) ) return "string" ;
      else if( typeid( _parameter ) == typeid( bool ) ) return "bool";
      
      else
	return typeid( _parameter ).name() ; 
    }
    
    virtual const std::string  defaultValue() {

     std::stringstream def ;

     //def << _defaultValue  ;
     toStream( def,  _parameter , setSize() )  ;

     return def.str() ;
    }

    virtual const std::string  value() {

     std::stringstream def ;

     //     def << _parameter  ;
     toStream( def,  _parameter , setSize() )  ;

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
