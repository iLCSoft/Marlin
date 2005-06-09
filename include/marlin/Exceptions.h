#ifndef Exceptions_h
#define Exceptions_h 1

#include "lcio.h"

// define some exceptions for Marlin

namespace marlin{

  /**ParseException used for parse errors, e.g. when reading the steering file.
   * @author gaede
   * @version $Id: Exceptions.h,v 1.1 2005-06-09 15:05:39 gaede Exp $
   */
  class ParseException : public lcio::Exception{
    
  protected:
    ParseException() {  /*no_op*/ ; } 
  public: 
    virtual ~ParseException() throw() { /*no_op*/; } 

    ParseException( std::string text ){
      message = "marlin::ParseException: " + text ;
    }
  }; 

    

} // end namespace 

#endif



