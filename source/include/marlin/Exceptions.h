#ifndef Exceptions_h
#define Exceptions_h 1

#include "lcio.h"
#include "Processor.h" 

// define some exceptions for Marlin

namespace marlin{

  /**ParseException used for parse errors, e.g. when reading the steering file.
   * 
   * @author gaede
   * @version $Id: Exceptions.h,v 1.5 2007-02-02 17:15:25 gaede Exp $
   */
  class ParseException : public lcio::Exception{
    
  protected:
    ParseException() = default;
  public: 
    virtual ~ParseException() noexcept override = default;

    ParseException( std::string text ){
      message = "marlin::ParseException: " + text ;
    }
  }; 

//    class Processor ;

  /**SkipEventException used to skip the current event in Processor::processEvent.
   * @author gaede
   * @version $Id: Exceptions.h,v 1.5 2007-02-02 17:15:25 gaede Exp $
   */
  class SkipEventException : public lcio::Exception{

  protected:
    SkipEventException() = default;

  public: 
    SkipEventException(const Processor* proc){
      message = proc->name()  ;
    }
    virtual ~SkipEventException() noexcept override = default;

  }; 

  /** StopProcessingException used to stop the current proccessing of events and 
   *  call Processor::end().
   * @author gaede
   * @version $Id: Exceptions.h,v 1.5 2007-02-02 17:15:25 gaede Exp $
   */
  class StopProcessingException : public lcio::Exception{

  protected:
    StopProcessingException() {  /*no_op*/ ; } 

  public: 
    StopProcessingException(const Processor* proc){
      message = proc->name()  ;
    }
    virtual ~StopProcessingException() noexcept override = default;

  }; 

  /** RewindDataFilesException used to stop the current proccessing of events, 
   *  rewind to the first event and restart the processing.
   * @author gaede
   * @version $Id: Exceptions.h,v 1.5 2007-02-02 17:15:25 gaede Exp $
   */
  class RewindDataFilesException : public lcio::Exception{

  protected:
    RewindDataFilesException() = default;

  public: 
    RewindDataFilesException(const Processor* proc){
      message = proc->name()  ;
    }
    virtual ~RewindDataFilesException() noexcept override = default;

  }; 

    

} // end namespace 

#endif



