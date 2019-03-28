#include <marlin/LoggerManager.h>

// -- marlin headers
#include <marlin/Application.h>

namespace marlin {

  LoggerManager::LoggerManager() {
    _mainLogger = Logging::createLogger( "main" ) ;
    _mainLogger->setLevel<MESSAGE>() ;
  }

  //--------------------------------------------------------------------------

  void LoggerManager::init( const Application *app ) {
    if ( _initialized ) {
      throw Exception( "LoggerManager::init: already initialized!" ) ;
    }
    auto globals = app->globalParameters() ;
    auto verbosityLevel = globals->getStringVal( "Verbosity" ) ;
    auto logFileName = globals->getStringVal( "LogFileName" ) ;
    auto coloredConsole = globals->getIntVal( "ColoredConsole" ) ;
    streamlog::logsink_list sinks {} ;
    if ( coloredConsole ) {
      sinks.push_back( streamlog::logstream::coloredConsole<Logging::mutex_type>() ) ;
    }
    else {
      sinks.push_back( streamlog::logstream::console<Logging::mutex_type>() ) ;
    }
    if ( not logFileName.empty() ) {
      sinks.push_back( streamlog::logstream::simpleFile<Logging::mutex_type>( logFileName ) ) ;
    }
    // configure both main and global logger 
    if ( not verbosityLevel.empty() ) {
      setLevel( verbosityLevel ) ;
    }
    mainLogger()->setName( app->program() ) ;
    mainLogger()->setSinks( sinks ) ;
    streamlog::logstream::global().setName( app->program() ) ;
    streamlog::logstream::global().setSinks( sinks ) ;
    _initialized = true ;
  }

  //--------------------------------------------------------------------------

  LoggerManager::Logger LoggerManager::mainLogger() const {
    return _mainLogger ;
  }

  //--------------------------------------------------------------------------

  LoggerManager::Logger LoggerManager::createLogger( const std::string &name ) const {
    if ( not isInitialized() ) {
      throw Exception( "LoggerManager::createLogger: not yet initialized!" ) ;
    }
    auto logger = Logging::createLogger( name ) ;
    logger->setSinks( mainLogger()->sinks() ) ;
    logger->setLevel( mainLogger()->levelName() ) ;
    return logger ;
  }

  //--------------------------------------------------------------------------

  bool LoggerManager::isInitialized() const {
    return _initialized ;
  }
  
  //--------------------------------------------------------------------------
  
  void LoggerManager::setLevel( const std::string &level ) {
    mainLogger()->setLevel( level ) ;
    streamlog::logstream::global().setLevel( level ) ;
  }

} // namespace marlin
