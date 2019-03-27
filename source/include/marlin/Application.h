#ifndef MARLIN_APPLICATION_h
#define MARLIN_APPLICATION_h 1

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/IParser.h"
#include "marlin/Logging.h"

namespace marlin {

  /**
   *  @brief  Application class
   *  Base application interface for running a Marlin application.
   *  See daughter classes for details.
   */
  class Application {
  public:
    // traits
    using CmdLineArguments = std::vector<std::string> ;
    using Logger = Logging::Logger ;

  public:
    Application() = default ;
    virtual ~Application() = default ;

    /**
     *  @brief  Run the Marlin application
     */
    virtual void run() = 0 ;

  protected:
    /**
     *  @brief  Initialize the application
     */
    virtual void init() = 0;

  public:
    /**
     *  @brief  Initialize the application
     *
     *  @param  argc argc from main function
     *  @param  argv argv from main function
     */
    void init( int argc, char **argv ) ;

    /**
     *  @brief  Print command line usage
     */
    virtual void printUsage() const = 0;

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
    StringVec activeProcessors () const ;

    /**
     *  @brief  Get the active processor conditions list
     */
    StringVec processorConditions () const ;

    /**
     *  @brief  Whether the application has been initialized
     */
    bool isInitialized() const ;

    /**
     *  @brief  Get the application logger instance
     */
    Logger logger() const ;

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

  protected:
    /// The arguments from main function after command line arguments have been removed
    CmdLineArguments           _filteredArguments {} ;

  private:
    /// The program name. Initialized on init()
    std::string                _programName {} ;
    /// The arguments from main function
    CmdLineArguments           _arguments {} ;
    /// The command line parameter arguments (parameters and constants)
    CommandLineParametersMap   _cmdLineOptions {} ;
    /// Whether the application has been initialized
    bool                       _initialized {false} ;
    /// Whether the verbosity level has been set using a cmd line argument
    bool                       _verbosityFromCmdLine {false} ;
    /// The steering file name
    std::string                _steeringFileName {} ;
    /// The XML steering file parser
    std::shared_ptr<IParser>   _parser {nullptr} ;
    /// The application logger
    Logger                     _logger {nullptr} ;
  };

} // end namespace marlin

#endif
