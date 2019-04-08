#ifndef MARLIN_LOGGING_h
#define MARLIN_LOGGING_h

// -- streamlog headers
#include <streamlog/streamlog.h>

namespace marlin {
  
  // import the log level classes into the marlin namespace 
  using  streamlog::DEBUG ;
  using  streamlog::DEBUG0 ;
  using  streamlog::DEBUG1 ;
  using  streamlog::DEBUG2 ;
  using  streamlog::DEBUG3 ;
  using  streamlog::DEBUG4 ;
  using  streamlog::DEBUG5 ;
  using  streamlog::DEBUG6 ;
  using  streamlog::DEBUG7 ;
  using  streamlog::DEBUG8 ;
  using  streamlog::DEBUG9 ;
  using  streamlog::MESSAGE ;
  using  streamlog::MESSAGE0 ;
  using  streamlog::MESSAGE1 ;
  using  streamlog::MESSAGE2 ;
  using  streamlog::MESSAGE3 ;
  using  streamlog::MESSAGE4 ;
  using  streamlog::MESSAGE5 ;
  using  streamlog::MESSAGE6 ;
  using  streamlog::MESSAGE7 ;
  using  streamlog::MESSAGE8 ;
  using  streamlog::MESSAGE9 ;
  using  streamlog::WARNING ;
  using  streamlog::WARNING0 ;
  using  streamlog::WARNING1 ;
  using  streamlog::WARNING2 ;
  using  streamlog::WARNING3 ;
  using  streamlog::WARNING4 ;
  using  streamlog::WARNING5 ;
  using  streamlog::WARNING6 ;
  using  streamlog::WARNING7 ;
  using  streamlog::WARNING8 ;
  using  streamlog::WARNING9 ;
  using  streamlog::ERROR ;
  using  streamlog::ERROR0 ;
  using  streamlog::ERROR1 ;
  using  streamlog::ERROR2 ;
  using  streamlog::ERROR3 ;
  using  streamlog::ERROR4 ;
  using  streamlog::ERROR5 ;
  using  streamlog::ERROR6 ;
  using  streamlog::ERROR7 ;
  using  streamlog::ERROR8 ;
  using  streamlog::ERROR9 ;
  using  streamlog::SILENT ;

  /**
   *  @brief  Loggin class.
   *  Defines the basic logging functionalities in Marlin.
   *  For a more application oriented configuration, see LoggerManager.h
   */
  class Logging {
  public:
#ifdef MARLIN_LOGGER_TS
    using mutex_type = streamlog::mt ;
#else
    using mutex_type = streamlog::st ;
#endif
    using Logger = std::shared_ptr<streamlog::logstreamT<mutex_type>> ;
    using StreamType = Logger::element_type::stream_type ;

  public:
    /**
     *  @brief  Create a standalone logger instance
     *
     *  @param  name the logger name
     */
    static Logger createLogger( const std::string &name ) ;

    /**
     *  @brief  Create a new logger using the sinks from the application
     *
     *  @param  name the logger name
     *  @param  app the application from which to get the sink
     */
    // static Logger createLogger( const std::string &name , const Application &app ) ;

  };

}

#endif
