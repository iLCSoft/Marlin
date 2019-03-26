#ifndef MARLIN_LOGGING_h
#define MARLIN_LOGGING_h

// -- streamlog headers
#include <streamlog/streamlog.h>

// -- marlin headers
#include <marlin/VerbosityLevels.h>  // import verbosity levels

namespace marlin {

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
