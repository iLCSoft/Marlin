#ifndef Exceptions_h
#define Exceptions_h 1

#include "lcio.h"

// define some exceptions for Marlin

namespace marlin{

  /**ParseException used for parse errors, e.g. when reading the steering file.
   * @author gaede
   * @version $Id: Exceptions.h,v 1.2 2005-06-09 16:09:14 gaede Exp $
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

   class Processor ;

  /**SkipEventException used to skip the current event in Processor::processEvent.
   * @author gaede
   * @version $Id: Exceptions.h,v 1.2 2005-06-09 16:09:14 gaede Exp $
   */
  class SkipEventException : public lcio::Exception{

  protected:
    SkipEventException() {  /*no_op*/ ; } 

  public: 
    SkipEventException(Processor* proc){
      message = proc->name()  ;
    }
    virtual ~SkipEventException() throw() { /*no_op*/; } 

  }; 

  /** StopProcessingException used to stop the current proccessing of events and 
   *  call Processor::end().
   * @author gaede
   * @version $Id: Exceptions.h,v 1.2 2005-06-09 16:09:14 gaede Exp $
   */
  class StopProcessingException : public lcio::Exception{

  protected:
    StopProcessingException() {  /*no_op*/ ; } 

  public: 
    StopProcessingException(Processor* proc){
      message = proc->name()  ;
    }
    virtual ~StopProcessingException() throw() { /*no_op*/; } 

  }; 

    

} // end namespace 

#endif



