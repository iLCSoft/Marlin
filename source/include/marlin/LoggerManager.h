#ifndef MARLIN_LOGGERMANAGER_h
#define MARLIN_LOGGERMANAGER_h 1

// -- std headers
#include <string>

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/Logging.h"

namespace marlin {

  class Application ;

  /**
   *  @brief  LoggerManager class
   *  Responsible for configuring logger for a given application.
   *  Can possibly configure the global logger instance.
   */
  class LoggerManager {
  public:
    using Logger = Logging::Logger ;

  public:
    LoggerManager(const LoggerManager &) = delete ;
    LoggerManager& operator=(const LoggerManager &) = delete ;
    ~LoggerManager() = default ;

    /**
     *  @brief  Constructor
     */
    LoggerManager() ;

    /**
     *  @brief  Initialize the manager and the manager
     *  using the application settings
     *
     *  @param app the application
     */
    void init( const Application *app ) ;

    /**
     *  @brief  Get the main logger instance (not the global one)
     */
    Logger mainLogger() const ;

    /**
     *  @brief  Create a new logger instance.
     *  The logger shares the same sinks as the main logger
     *
     *  @param  name the logger name
     */
    Logger createLogger( const std::string &name ) const ;

    /**
     *  @brief  Whether the logger manager has been initialized
     */
    bool isInitialized() const ;

  private:
    /// The main logger instance
    Logger             _mainLogger {nullptr} ;
    /// Whether the manager has been initialized
    bool               _initialized {false} ;
  };

} // end namespace marlin

#endif
