#pragma once

// -- std headers
#include <exception>
#include <string>

// -- high level exception macros
#define MARLIN_THROW( message ) throw marlin::Exception( __LINE__, __FUNCTION__, __FILE__, message )
#define MARLIN_THROW_T( ExceptionType, message ) throw ExceptionType( __LINE__, __FUNCTION__, __FILE__, message )
#define MARLIN_RETHROW( orig, message ) throw marlin::Exception( orig, __LINE__, __FUNCTION__, __FILE__, message )
#define MARLIN_RETHROW_T( ExceptionType, orig, message ) throw ExceptionType( orig, __LINE__, __FUNCTION__, __FILE__, message )

// -- specific marlin exception
#define MARLIN_SKIP_EVENT( proc ) MARLIN_THROW_T( marlin::SkipEventException, proc->name() )
#define MARLIN_STOP_PROCESSING( proc ) MARLIN_THROW_T( marlin::StopProcessingException, proc->name() )

// -- custom exception class definition macro
#define MARLIN_DEFINE_EXCEPTION( ClassName ) \
  class ClassName : public marlin::Exception { \
  public: \
    ClassName() = delete ; \
    ClassName( const ClassName & ) = default ; \
    ~ClassName() = default ; \
    inline ClassName( const std::string &message ) : \
      marlin::Exception( std::string( #ClassName ) + " - " + message ) {} \
    inline ClassName( unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) : \
      marlin::Exception( line, func, fname, std::string( #ClassName ) + " - " + message ) {} \
    template <typename T> \
    inline ClassName( const T &rhs, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) : \
      marlin::Exception( rhs, line, func, fname, std::string( #ClassName ) + " - " + message ) {} \
  }

namespace marlin {
  
  /**
   *  @brief  Exception class.
   *
   *  Smart exception class. Tracking of:
   *  - file name
   *  - line number
   *  - function name
   *  of the exception location. The exception message can be stack 
   *  with any other previously thrown exception.
   *  Use the pre-processor macro to throw an exception:
   *  @code{cpp}
   *  MARLIN_THROW( "A problem occured" );
   *  @endcode
   *  An exception can rethrown:
   *  @code{cpp}
   *  try {
   *    // ... some code
   *  }
   *  catch ( const marlin::Exception &e ) {
   *    MARLIN_RETHROW( e, "A problem occured" );
   *  }
   *  @endcode
   *  In this case, the message will appended to the previous exception message
   *  
   */
  class Exception : public std::exception {
  public:
    Exception() = delete ;
    Exception( const Exception & ) = default ;
    virtual ~Exception() = default ;
    
    /**
     *  @brief  Constructor
     * 
     *  @param  message the exception message
     */
    Exception( const std::string &message ) ;
    
    /**
     *  @brief  Constructor
     *
     *  @param  line the exception line number
     *  @param  func the function name in which the exception has been thrown
     *  @param  fname the file in which the exception has been thrown
     *  @param  message the exception message
     */
    Exception( unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) ;
    
    /**
     *  @brief  Constructor
     *
     *  @param  rhs any possible class having a method what() returning a string
     *  @param  line the exception line number
     *  @param  func the function name in which the exception has been thrown
     *  @param  fname the file in which the exception has been thrown
     *  @param  message the exception message
     */
    template <typename T>
    Exception( const T &rhs, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) ;
    
    /**
     *  @brief  Get the full exception message
     */
    const char* what() const noexcept override ;

  protected:
    /**
     *  @brief  Helper function creating the full exception message
     *  
     *  @param  line the exception line number
     *  @param  func the function name in which the exception has been thrown
     *  @param  fname the file in which the exception has been thrown
     *  @param  message the exception message
     */
    std::string createMessage( unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) const ;
    
    /**
     *  @brief  Helper function creating the full exception message
     *
     *  @param  a previous message to prepend in the message (plus a line break)
     *  @param  line the exception line number
     *  @param  func the function name in which the exception has been thrown
     *  @param  fname the file in which the exception has been thrown
     *  @param  message the exception message
     */
    std::string createMessage( const std::string &previous, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) const ;
    
  protected:
    ///< The full exception message
    const std::string       _message {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline Exception::Exception( const T &rhs, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) :
    _message( createMessage(rhs.what(), line, func, fname, message) ) {
    /* nop */
  }
  
  /// Definition of Marlin exceptions
  MARLIN_DEFINE_EXCEPTION( SkipEventException ) ;
  MARLIN_DEFINE_EXCEPTION( StopProcessingException ) ;
  MARLIN_DEFINE_EXCEPTION( ParseException ) ;

} // end namespace
