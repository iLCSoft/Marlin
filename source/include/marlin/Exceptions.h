#ifndef Exceptions_h
#define Exceptions_h 1

#include <lcio.h>

namespace marlin {

  class Processor ;
  using Exception = lcio::Exception ;

  /**ParseException used for parse errors, e.g. when reading the steering file.
   *
   * @author gaede
   * @version $Id: Exceptions.h,v 1.5 2007-02-02 17:15:25 gaede Exp $
   */
  class ParseException : public Exception {
  protected:
    ParseException() = delete ;
  public:
    virtual ~ParseException() throw() = default ;
    ParseException( const std::string &text ) ;
  };

  /**SkipEventException used to skip the current event in Processor::processEvent.
   * @author gaede
   * @version $Id: Exceptions.h,v 1.5 2007-02-02 17:15:25 gaede Exp $
   */
  class SkipEventException : public Exception {

  protected:
    SkipEventException() = delete ;

  public:
    virtual ~SkipEventException() throw() = default ;
    SkipEventException( const Processor* proc ) ;
  };

  /** StopProcessingException used to stop the current proccessing of events and
   *  call Processor::end().
   * @author gaede
   * @version $Id: Exceptions.h,v 1.5 2007-02-02 17:15:25 gaede Exp $
   */
  class StopProcessingException : public Exception{
  protected:
    StopProcessingException() = delete ;

  public:
    virtual ~StopProcessingException() throw() = default ;
    StopProcessingException(const Processor* proc) ;
  };

  /** RewindDataFilesException used to stop the current proccessing of events,
   *  rewind to the first event and restart the processing.
   * @author gaede
   * @version $Id: Exceptions.h,v 1.5 2007-02-02 17:15:25 gaede Exp $
   */
  class RewindDataFilesException : public Exception{
  protected:
    RewindDataFilesException() = delete ;

  public:
    virtual ~RewindDataFilesException() throw() = default ;
    RewindDataFilesException(const Processor* proc) ;
  };

} // end namespace

#endif
