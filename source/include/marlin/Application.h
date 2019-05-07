#ifndef MARLIN_APPLICATION_h
#define MARLIN_APPLICATION_h 1

// -- marlin headers
#include <marlin/Exceptions.h>
#include <marlin/IParser.h>
#include <marlin/Logging.h>
#include <marlin/GeometryManager.h>
#include <marlin/LoggerManager.h>
#include <marlin/RandomSeedManager.h>

namespace EVENT {
  class LCEvent ;
  class LCRunHeader ;
}

namespace marlin {
  
  class IScheduler ;
  class DataSourcePlugin ;

  /**
   *  @brief  Application class
   *  Base application interface for running a Marlin application.
   *  See daughter classes for details.
   */
  class Application {
  public:
    using CmdLineArguments = std::vector<std::string> ;
    using Logger = Logging::Logger ;
    using Scheduler = std::shared_ptr<IScheduler> ;
    using EventList = std::vector<std::shared_ptr<EVENT::LCEvent>> ;
    using DataSource = std::shared_ptr<DataSourcePlugin> ;
    using ConditionsMap = std::map<std::string, std::string> ;

  public:
    Application() = default ;
    ~Application() = default ;

  public:
    /**
     *  @brief  Initialize the application
     *
     *  @param  argc argc from main function
     *  @param  argv argv from main function
     */
    void init( int argc, char **argv ) ;

    /**
     *  @brief  Run the application
     */
    void run() ;

    /**
     *  @brief  Print command line usage
     */
    void printUsage() const ;

    /**
     *  @brief  Print the Marlin banner
     *
     *  @param  out the out stream for printout
     */
    void printBanner( std::ostream &out = std::cout ) const ;

    /**
     *  @brief  Get the program name
     */
    const std::string &program() const { return _programName ; }

    /**
     *  @brief  Get the command line arguments
     */
    const CmdLineArguments &arguments() const { return _arguments ; }

    /**
     *  @brief  Get the global section parameters
     */
    std::shared_ptr<StringParameters> globalParameters () const ;

    /**
     *  @brief  Get the geometry section parameters
     */
    std::shared_ptr<StringParameters> geometryParameters () const ;

    /**
     *  @brief  Get the data source section parameters
     */
    std::shared_ptr<StringParameters> dataSourceParameters () const ;

    /**
     *  @brief  Get the processor parameters
     *
     *  @param  name the processor name
     */
    std::shared_ptr<StringParameters> processorParameters ( const std::string &name ) const ;

    /**
     *  @brief  Get the XML constants
     */
    std::shared_ptr<StringParameters> constants () const ;

    /**
     *  @brief  Get the active processor list
     */
    std::vector<std::string> activeProcessors () const ;

    /**
     *  @brief  Get the active processor conditions list
     */
    std::vector<std::string> processorConditions () const ;

    /**
     *  @brief  Whether the application has been initialized
     */
    bool isInitialized() const ;

    /**
     *  @brief  Get the application logger instance
     */
    Logger logger() const ;

    /**
     *  @brief  Create a new logger instance.
     *  If the logger manager is initialized, the logger
     *  will share the same sinks as the main logger,
     *  else a fresh new logger is created.
     *
     *  @param  name the logger name
     */
    Logger createLogger( const std::string &name ) const ;

    /**
     *  @brief  Get the geometry manager
     */
    const GeometryManager &geometryManager() const ;
    
    /**
     *  @brief  Get the random seed manager
     */
    const RandomSeedManager &randomSeedManager() const ;
    
    /**
     *  @brief  Get the random seed manager
     */
    RandomSeedManager &randomSeedManager() ;
    
    /**
     *  @brief  Set the scheduler instance to use in this application.
     *  Must be called before init(argc, argv)
     *  
     *  @param  scheduler the sceduler instance to use
     */
    void setScheduler( Scheduler scheduler ) ;

  protected:
    /**
     *  @brief  Get the parser instance
     */
    std::shared_ptr<IParser> parser() const ;

  private:
    /**
     *  @brief  Parse the command line arguments
     */
    void parseCommandLine() ;

    /**
     *  @brief  Create the parser instance based on the steering file extension
     */
    std::shared_ptr<IParser> createParser() const ;
    
    /**
     *  @brief  Callback function to process an event received from the data source
     * 
     *  @param  event the event to process
     */
    void onEventRead( std::shared_ptr<EVENT::LCEvent> event ) ;
    
    /**
     *  @brief  Callback function to process a run header received from the data source
     * 
     *  @param  rhdr the run header to process
     */
    void onRunHeaderRead( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;
    
    /**
     *  @brief  Processed finished events from the output queue
     *
     *  @param  events the list of finished events
     */
    void processFinishedEvents( const EventList &events ) const ;

  protected:
    /// The arguments from main function after command line arguments have been removed
    CmdLineArguments           _filteredArguments {} ;
    /// The geometry manager
    GeometryManager            _geometryMgr {} ;
    /// The random seed manager
    RandomSeedManager          _randomSeedMgr {} ;
    /// The logger manager
    LoggerManager              _loggerMgr {} ;

  private:
    /// The program name. Initialized on init()
    std::string                _programName {} ;
    /// The arguments from main function
    CmdLineArguments           _arguments {} ;
    /// The command line parameter arguments (parameters and constants)
    CommandLineParametersMap   _cmdLineOptions {} ;
    /// Whether the application has been initialized
    bool                       _initialized {false} ;
    /// The steering file name
    std::string                _steeringFileName {} ;
    /// The XML steering file parser
    std::shared_ptr<IParser>   _parser {nullptr} ;
    ///< The event processing scheduler instance
    Scheduler                  _scheduler {nullptr} ;
    ///< The data source plugin
    DataSource                 _dataSource {nullptr} ;
    ///< Initial processor runtime conditions from steering file
    ConditionsMap              _conditions {} ;
    ///< Whether the currently pushed event is the first one
    bool                       _isFirstEvent {true} ;
  };

} // end namespace marlin

#endif
