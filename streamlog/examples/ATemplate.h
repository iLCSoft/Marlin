#ifndef ATemplate_h
#define ATemplate_h

#include <typeinfo>

namespace streamlog_test{

  template<class T>
  class ATemplate {
    
  public:
    
  ATemplate() {} ; 
  
  void doIt() { 

    streamlog_out_T( DEBUG0 ) << " Hello World from a templated class with templated type: [" 
			      << typeid( T ).name()  << "] " 
			      << std::endl; 
    
    streamlog_message_T( DEBUG0 ,
			 float pi = 3.0 + .141592 ; ,
			 " message from templated class with code - computed pi: " << pi << std::endl ;
			 )
      
      }
    
  };
  
}
#endif
